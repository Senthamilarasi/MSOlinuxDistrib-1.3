#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "libMSO.h"
#include "drv_rs232.h"
#include "sp_rs232.h"
#include "Sp_Rs232_Internal.h"
#include "Crc.h"

#define ONE_FRAME					0
#define	ALL_FRAMES					1

/*******************************************************************************
-- macros definition
*******************************************************************************/
#ifndef MAX
	#define MAX(a,b) ((( a )>( b )) ? ( a ):( b ))
#endif

#ifndef MIN
	#define MIN(a,b) ((( a )<( b )) ? ( a ):( b ))
#endif

/*******************************************************************************
-- static function declaration
*******************************************************************************/

static void s_SPRS232_MakeSOP( UC l_uc_Type,
							   UC l_b_First,
							   UC l_b_Last,
							   UC i_uc_RC,
							   PUC o_puc_PacketToSend, 
							   PUL o_pul_PacketCurrentSize);

static void s_SPRS232_AddDataToPacket(PUC io_puc_PacketToSend,
									  PUL io_pul_PacketCurrentSize,
									  PUC i_puc_Data,
									  UL  i_ul_SizeOfSendData);

static void	s_SPRS232_AddEOP(PUC io_puc_PacketToSend, PUL io_pul_PacketCurrentSize);

static I s_SPRS232_ReceiveSOP(UC *o_uc_Type,
								   UC *o_b_First, 
								   UC *o_b_Last, 
								   UC * o_puc_RC, 
								   UL i_n_TimeOut);

static I s_SPRS232_ReadOneByte(
						PUC	o_puc_Data);

static I s_SPRS232_ReceivePacket(UC *o_puc_Data , UL *o_pul_Size, UL i_ul_BufferSize);


/*******************************************************************************
-- static variable declaration
*******************************************************************************/
static UC	gs_uc_RequestCounter	= 0;
static B	g_b_SPRS232IsInit		= FALSE;

typedef struct
{
	UL	l_ul_NoUse;
} T_SPRS232_STRUCT, *PT_SPRS232_STRUCT;

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//					STATIC FUNCTIONS IMPLEMENTATION								//
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////


//+---------------------------------------------------------------------------
//
//  Function:   s_SPRS232_MakeSOP implementation
//
//  Synopsis:	This function make the SOP
//							
//
//  Arguments:	UC l_uc_Type	: Type (DATA, ACK or NACK)
//				UC l_b_First	: TRUE or FALSE
//				UC l_b_Last		: TRUE or FALSE
//				UC i_uc_RC		: Request Counter
//				PUC o_puc_PacketToSend : SOP
//				PUL o_pul_PacketCurrentSize : size of the SOP
//
//
//	Returned value :	
//
//  History:
//
//  Notes:	
//	
//----------------------------------------------------------------------------
void s_SPRS232_MakeSOP(UC l_uc_Type, 
					   UC l_b_First, 
					   UC l_b_Last, 
					   UC i_uc_RC, 
					   PUC o_puc_PacketToSend, 
					   PUL o_pul_PacketCurrentSize)
{
	UC l_uc_Byte;

	//  ajout STX
	o_puc_PacketToSend[0] = STX;

	//  ajout ID
	l_uc_Byte=l_uc_Type|(DIR_OUT&SPRS232_ID_TYP_PACKET_DIR);
	if (l_b_First)
	l_uc_Byte |= SPRS232_ID_TYP_PACKET_FIRST;
	if (l_b_Last)
	l_uc_Byte |= SPRS232_ID_TYP_PACKET_LAST;

	o_puc_PacketToSend[1] = l_uc_Byte;

	//  stuffing sur RC et ajout de RC
	switch (i_uc_RC)
	{
		case DLE:
			o_puc_PacketToSend[2] = DLE;
			o_puc_PacketToSend[3] = DLE;
			*o_pul_PacketCurrentSize = 4;
			break;
		case XON:
			o_puc_PacketToSend[2] = DLE;
			o_puc_PacketToSend[3] = XON+1;
			*o_pul_PacketCurrentSize = 4;
			break;
		case XOFF:
			o_puc_PacketToSend[2] = DLE;
			o_puc_PacketToSend[3] = XOFF+1;
			*o_pul_PacketCurrentSize = 4;
			break;
		default:
			o_puc_PacketToSend[2] = i_uc_RC;
			*o_pul_PacketCurrentSize = 3;
	}
}

//+---------------------------------------------------------------------------
//
//  Function:   s_SPRS232_AddDataToPacket implementation
//
//  Synopsis:	This function add data in the packet
//							
//
//  Arguments:	PUC io_puc_PacketToSend	: the packet to modify
//				PUL io_pul_PacketCurrentSize : size of the packet
//				PUC i_puc_Data	: data to add
//				UL  i_ul_SizeOfSendData : size of the data
//
//
//	Returned value :	
//
//  History:
//
//  Notes:	
//	
//----------------------------------------------------------------------------
void s_SPRS232_AddDataToPacket(PUC io_puc_PacketToSend,
							   PUL io_pul_PacketCurrentSize,
							   PUC i_puc_Data,
							   UL  i_ul_SizeOfSendData)
{
	UL i;
	UC l_auc_CRC[2] = {0x00,0x00};

	// Calcul du CRC
	ComputeCrc16(i_puc_Data,i_ul_SizeOfSendData,l_auc_CRC+1,l_auc_CRC);

	// Ajout des Data en faisant le stuffing
	for(i=0;i<i_ul_SizeOfSendData;i++)
	{
		switch (i_puc_Data[i])
		{
			case DLE:
				io_puc_PacketToSend[(*io_pul_PacketCurrentSize)++] = DLE;
				io_puc_PacketToSend[(*io_pul_PacketCurrentSize)++] = DLE;
				break;
			case XON:
				io_puc_PacketToSend[(*io_pul_PacketCurrentSize)++] = DLE;
				io_puc_PacketToSend[(*io_pul_PacketCurrentSize)++] = XON+1;
				break;
			case XOFF:
				io_puc_PacketToSend[(*io_pul_PacketCurrentSize)++] = DLE;
				io_puc_PacketToSend[(*io_pul_PacketCurrentSize)++] = XOFF+1;
				break;
			default:
				io_puc_PacketToSend[(*io_pul_PacketCurrentSize)++] = i_puc_Data[i];
				break;
		}
	}

	// Ajout du CRC en faisant le Stuffing
	for(i=0;i<2;i++)
	{
		switch (l_auc_CRC[i])
		{
			case DLE:
				io_puc_PacketToSend[(*io_pul_PacketCurrentSize)++] = DLE;
				io_puc_PacketToSend[(*io_pul_PacketCurrentSize)++] = DLE;
				break;
			case XON:
				io_puc_PacketToSend[(*io_pul_PacketCurrentSize)++] = DLE;
				io_puc_PacketToSend[(*io_pul_PacketCurrentSize)++] = XON+1;
				break;
			case XOFF:
				io_puc_PacketToSend[(*io_pul_PacketCurrentSize)++] = DLE;
				io_puc_PacketToSend[(*io_pul_PacketCurrentSize)++] = XOFF+1;
				break;
			default:
				io_puc_PacketToSend[(*io_pul_PacketCurrentSize)++] = l_auc_CRC[i];
				break;
		}
	}
}

//+---------------------------------------------------------------------------
//
//  Function:   s_SPRS232_AddEOP implementation
//
//  Synopsis:	This function add the End Of the Packet
//							
//
//  Arguments:	PUC io_puc_PacketToSend	: the packet to modify
//				PUL io_pul_PacketCurrentSize : size of the packet
//
//
//	Returned value :	
//
//  History:
//
//  Notes:	
//	
//----------------------------------------------------------------------------
void s_SPRS232_AddEOP(PUC io_puc_PacketToSend, PUL io_pul_PacketCurrentSize)
{
	io_puc_PacketToSend[(*io_pul_PacketCurrentSize)++] = DLE;
	io_puc_PacketToSend[(*io_pul_PacketCurrentSize)++] = ETX;
}

//+---------------------------------------------------------------------------
//
//  Function:   s_SPRS232_UnStuffing implementation
//
//  Synopsis:	This function find data after DLE
//							
//
//  Arguments:	PUC io_puc_PacketToSend	: the packet to modify
//				PUL io_pul_PacketCurrentSize : size of the packet
//
//
//	Returned value :	
//
//  History:
//
//  Notes:	
//	
//----------------------------------------------------------------------------
int s_SPRS232_UnStuffing(PUC	o_puc_Data)
{
	switch (*o_puc_Data)
	{
		case DLE:
			break;
		case XON+1:
			*o_puc_Data = XON;	// <XON>
			break;
		case XOFF+1:
			*o_puc_Data = XOFF;	// <XOFF>
			break;
		case ETX:
			return SPRS232_ERR_EOP;
		default:
			return SPRS232_BAD_STUFFING;
			break;
	}

	return SPRS232_OK;
}

//+---------------------------------------------------------------------------
//
//  Function:   s_SPRS232_ReceiveSOP implementation
//
//  Synopsis:	This function wait SOP (beginning packet, ACK or NACK)
//							
//
//  Arguments:	UC *o_uc_Type	: return ACK, NACK, DATA
//				UC *o_b_First	: TRUE or FALSE
//				UC *o_b_Last	: TRUE or FALSE
//				UC * o_puc_RC	: Request Couter
//				DWORD i_dw_ReadTotalTimeoutConstant : (ms)
//
//
//	Returned value :	return SPRS232_OK	
//						return RS232ERR_READ_TIMEOUT
//
//  History:
//
//  Notes:	
//	
//----------------------------------------------------------------------------
I s_SPRS232_ReceiveSOP(UC *o_uc_Type,
							UC *o_b_First , 
							UC *o_b_Last, 
							UC * o_puc_RC, 
							DWORD i_dw_ReadTotalTimeoutConstant)
{
	I		l_nStatus;
	UC		l_uc_Byte;
	I		l_i_NbErr;
	I		l_i_State;
	DWORD	l_dw_Timeout;

////initialisation des arguments
	*o_uc_Type = 0xFF;
	*o_b_First = 0xFF;
	*o_b_Last = 0xFF;
	*o_puc_RC =  0xFF;
	l_uc_Byte =  0xFF;

	l_i_State		= SPRS232_WAIT_STX;
	l_dw_Timeout	= i_dw_ReadTotalTimeoutConstant;
	l_i_NbErr		= 0;
	
	while(l_i_State != SPRS232_WAIT_DATA)
	{
		l_nStatus = RS232_Read(&l_uc_Byte,1,l_dw_Timeout);

		if(l_nStatus)
		{
			if (l_nStatus==RS232ERR_READ_TIMEOUT)
			{
				if(l_i_NbErr)
				{
					return SPRS232_ERR_ERROR;
				}
				else 
				{
					return l_nStatus;
				}
			}

			l_i_NbErr ++;
			l_i_State		= SPRS232_WAIT_STX;
			l_dw_Timeout	= (DWORD)(SPRS232_TIMEOUT_ACK * 1.5);
		}
		else
		{

			switch ( l_i_State )
			{
			case SPRS232_WAIT_STX:
				if ( l_uc_Byte == STX ) 
				{
					l_i_State = SPRS232_WAIT_ID;
					l_dw_Timeout = 0;
				}
				break;

			case SPRS232_WAIT_ID:
				// c'est un packet IN ? 
				if (((l_uc_Byte&SPRS232_ID_TYP_PACKET_DIR)	==	DIR_IN)
					&&
				   ( ((l_uc_Byte & SPRS232_MASK_PACKET_TYPE) == SPRS232_ID_ID_PACKET_DATA) | ((l_uc_Byte & SPRS232_MASK_PACKET_TYPE) == SPRS232_ID_ID_PACKET_ACK) | ((l_uc_Byte & SPRS232_MASK_PACKET_TYPE) == SPRS232_ID_ID_PACKET_NACK) ) )
				{
					*o_b_First = l_uc_Byte & SPRS232_ID_TYP_PACKET_FIRST;
					*o_b_Last = l_uc_Byte & SPRS232_ID_TYP_PACKET_LAST;
					*o_uc_Type = l_uc_Byte & SPRS232_MASK_PACKET_TYPE;// on ne retourne que l'info PACKET TYPE
	
					l_i_State = SPRS232_WAIT_RC;
				}
				else
				{
					l_i_State		= SPRS232_WAIT_STX;
					l_dw_Timeout	= SPRS232_TIMEOUT_ACK;
				}
				break;

			case SPRS232_WAIT_RC:

				if (l_uc_Byte == DLE)
				{
					l_i_State = SPRS232_WAIT_RC_AFTER_DLE;
				}
				else
				{
					* o_puc_RC = l_uc_Byte;
					l_i_State = SPRS232_WAIT_DATA;
				}
				break;

			case SPRS232_WAIT_RC_AFTER_DLE:
				l_nStatus = s_SPRS232_UnStuffing(&l_uc_Byte);

				if (l_nStatus)
				{
					l_i_State		= SPRS232_WAIT_STX;
					l_dw_Timeout	= SPRS232_TIMEOUT_ACK;
				}
				else
				{
					* o_puc_RC = l_uc_Byte;
					l_i_State = SPRS232_WAIT_DATA;
				}
				break;
			default:
				return RS232ERR_ERROR;
			}
		}
	}
	
	return SPRS232_OK;
}

//+---------------------------------------------------------------------------
//
//  Function:   s_SPRS232_ReadOneByte implementation
//
//  Synopsis: This fonction read one byte.
//							
//
//  Arguments:	UC *o_puc_Data				: return the read data
//				UL i_ul_DataSizeToReturn	: number of the byte to read (or size of the buffer)
//				UL * o_pul_BytesReadILV		: number of the byte read
//				UC * o_puc_CRCH				: CRC Hight
//				UC * o_puc_CRCL				: CRC Low
//				INT i_iVerifyETX			: TRUE or FALSE
//
//  History:
//
//  Notes:	
//	
//----------------------------------------------------------------------------
I s_SPRS232_ReadOneByte(
						PUC	o_puc_Data)
{
	I	l_n_Status;

	l_n_Status = RS232_Read(o_puc_Data, 1,0);

	if (l_n_Status)
		return l_n_Status;

	if (*o_puc_Data == DLE)
	{
		l_n_Status = RS232_Read(o_puc_Data, 1,0);

		if (l_n_Status)
			return l_n_Status;
		
		l_n_Status = s_SPRS232_UnStuffing(o_puc_Data);

		if (l_n_Status)
			return l_n_Status;
		
	}
	return SPRS232_OK;
}

//+---------------------------------------------------------------------------
//
//  Function:   s_SPRS232_ReceivePacket implementation
//
//  Synopsis: The sop is received, then this fonction read while the End Of the packet
//			  isn't. The CRC is verify
//							
//
//  Arguments:	UC *o_puc_Data				: return the read data
//				UUL *o_pul_Size				: number of the byte read
//				UL i_ul_BufferSize			: size of the buffer
//
//
//	Returned value :	return SPRS232_OK	
//						return SPRS232_ERR_TIMEOUT
//						return SPRS232_ERR_CRC
//
//  History:
//
//  Notes:	
//	
//----------------------------------------------------------------------------
I s_SPRS232_ReceivePacket(UC *o_puc_Data , UL *o_pul_Size, UL i_ul_BufferSize)
{
	UC		l_uc_CRCH=0;		// MSB CRC16
	UC		l_uc_CRCL=0;		// LSB CRC16
	UC		l_uc_CRCH_Mess=0;	// MSB CRC16
	UC		l_uc_CRCL_Mess=0;	// LSB CRC16
	I	l_nStatus;
	I		l_i_BytesReadILV;

	*o_pul_Size = 0;

	l_i_BytesReadILV = -1;	
	do
	{
		l_i_BytesReadILV ++;

		if(l_i_BytesReadILV >= (I)i_ul_BufferSize)
			return SPRS232_ERR_SMALL_BUF;

		l_nStatus = s_SPRS232_ReadOneByte(o_puc_Data+l_i_BytesReadILV);
	}
	while(!l_nStatus);

	if(l_nStatus != SPRS232_ERR_EOP)
		return l_nStatus;

	l_uc_CRCH_Mess = o_puc_Data[--l_i_BytesReadILV];
	l_uc_CRCL_Mess = o_puc_Data[--l_i_BytesReadILV];

	*o_pul_Size =l_i_BytesReadILV;

	// vérification du CRC
	ComputeCrc16((PUC)o_puc_Data , 
						 (UL)(*o_pul_Size), 
						 &l_uc_CRCH,
						 &l_uc_CRCL);		
	
	if ((l_uc_CRCL_Mess != l_uc_CRCL) || (l_uc_CRCH_Mess != l_uc_CRCH))
	{
		*o_pul_Size=0;
		return SPRS232_ERR_CRC;
	}
	
	return SPRS232_OK;
}


//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//					EXPORT FUNCTIONS IMPLEMENTATION								//
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//+---------------------------------------------------------------------------
//
//  Function:   SPRS232_IsInitialize implementation
//
//  Synopsis: Check if the commmunication link has been created
//							
//
//  Arguments:	
//
//	Returned value :	TRUE if the port COM handle has been initialized
//						FALSE otherwise
//
//
//  Notes:	
//	
//----------------------------------------------------------------------------

BOOL SPRS232_IsInitialize
(
	MORPHO_HANDLE	i_h_Handle
)
{
	 return RS232_IsInitialize();
}

I SPRS232_OpenEx
( 
	MORPHO_HANDLE	*o_ph_Handle,
	PC		i_pc_String,
	UL		i_ul_BaudRate,
	PVOID	io_pv_Param
)
{
	return SPRS232_Initialize(	o_ph_Handle,
								i_pc_String,	//C * i_strName,
								i_ul_BaudRate	//UL i_ul_BaudRate)
								);
}

//+---------------------------------------------------------------------------
//
//  Function:   SPRS232_Initialize implementation
//
//  Synopsis: This fonction initialize the communication port
//							
//
//  Arguments:	C * i_strName	: communication port name
//				UL i_ul_BaudRate : communication Speed
//
//
//	Returned value :	return SPRS232_OK	
//						
//
//  History:
//
//  Notes:	
//	
//----------------------------------------------------------------------------
I SPRS232_Initialize
(
	MORPHO_HANDLE	*o_ph_Handle,
	C * i_strName,
	UL i_ul_BaudRate
)
{
	I	l_i_Ret;

	T_SPRS232_STRUCT	* l_px_SpRs232_Struct;


	if(g_b_SPRS232IsInit)
	{
		SPRS232_Close(	o_ph_Handle );
	}

	l_px_SpRs232_Struct = (T_SPRS232_STRUCT *) malloc ( sizeof ( T_SPRS232_STRUCT ) );
	if ( l_px_SpRs232_Struct == NULL )
	{	
		l_i_Ret = MSO_ERROR_MEMORY;
	}

	if (RS232_IsInitialize())
	{
		l_i_Ret = RS232_Close();
		if (l_i_Ret)
		{
			return l_i_Ret;
		}
	}

	l_i_Ret = RS232_Initialize(i_strName,XON,XOFF,i_ul_BaudRate);
	
	if(l_i_Ret)
	{
		return l_i_Ret;
	}

	g_b_SPRS232IsInit = TRUE;

	*o_ph_Handle = l_px_SpRs232_Struct;

	return SPRS232_OK;
}

//+---------------------------------------------------------------------------
//
//  Function:   SPRS232_SetBaudRate
//
//  Synopsis:	This function set : 	DCB
//										COMMTIMEOUTS
//									
//
//
//  Arguments:	DWORD	i_dw_BaudRate : 9600 bauds		: CBR_9600
//										.........
//										115200 bauds	: CBR_115200
//
//
//	Returned value :	RS232_OK
//						RS232ERR_ERROR
//
//  History: 
//
//  Notes:		
//
//----------------------------------------------------------------------------
I SPRS232_SetBaudRate
(	
	MORPHO_HANDLE	i_h_Handle,
	DWORD	i_dw_BaudRate
)
{
	I	l_i_Ret;

	if(!g_b_SPRS232IsInit)
		return SPRS232_ERR_INIT;

	Sleep(100);		//attente de la fin d'émission de la FIFO

	l_i_Ret = RS232_SetBaudRate(XON,XOFF,i_dw_BaudRate);
	if (l_i_Ret)
	{
		return l_i_Ret;
	}

	return SPRS232_OK;
}

//+---------------------------------------------------------------------------
//
//  Function:   SPRS232_Close implementation
//
//  Synopsis: This fonction close the communication port
//							
//
//  Arguments:	
//
//
//	Returned value :	return SPRS232_OK	
//						
//
//  History:
//
//  Notes:	
//	
//----------------------------------------------------------------------------
I SPRS232_Close
(
	MORPHO_HANDLE	*io_ph_Handle
)
{
	I	l_i_Ret;

	if(!g_b_SPRS232IsInit)
		return SPRS232_OK;

	Sleep(100);		//attente de la fin d'émission de la FIFO

	l_i_Ret = RS232_Close();

	g_b_SPRS232IsInit = FALSE;

	if ( *io_ph_Handle != NULL )
	{
		free ( *io_ph_Handle );
		*io_ph_Handle = NULL;
	}

	return SPRS232_OK;
}

I SPRS232_SendEx
(
	MORPHO_HANDLE	i_h_Handle,
	UL		i_ul_Timeout,
	PUC		i_puc_Data,
	UL		i_ul_DataSize
)
{
 return SPRS232_Send(	i_h_Handle,
						 i_puc_Data,			//UC * i_puc_Data , 
						 i_ul_DataSize		//UL i_ul_DataSize)
						 );
}
//+---------------------------------------------------------------------------
//
//  Function:   SPRS232_Send implementation
//
//  Synopsis: This fonction send the packet
//							
//
//  Arguments:	UC * i_puc_Data		: data to send
//				UL i_ul_DataSize	: size of data
//
//
//	Returned value :	return SPRS232_OK	
//						return SPRS232_ERR_TIMEOUT
//						
//
//  History:
//
//  Notes:	
//	
//----------------------------------------------------------------------------
I SPRS232_Send
(
	MORPHO_HANDLE	i_h_Handle,
	UC * i_puc_Data, 
	UL i_ul_DataSize
)
{
	I l_nStatus;
	UC	l_ucID,l_uc_RC;
	UL	l_ul_Count = 0;
	UL l_ul_SizeOfSendData = 0;
	UC l_uc_Attempt;
	BOOLEAN l_bFirst,l_bLast;
	UC l_uc_MaxNumberOfAttempt = 3;	// 3 essais d'envoi de paquet ( valeur par default)

	// 1 (RC) + 1024 (data) + 2 (CRC) = 1027, 
	// fois 2 pour le stuffing, + 4 (STX ID et DLE ETX) = 2058
	UC l_auc_PacketToSend[2058];
	UL l_ul_PacketCurrentSize = 0;
	
	if(!g_b_SPRS232IsInit)
		return SPRS232_ERR_INIT;

	do
	{
		// découpe des datas en paquets de taille <= MAX_DATA_SIZE
		l_ul_SizeOfSendData = MIN(MAX_DATA_SIZE,(i_ul_DataSize - l_ul_Count));
		
		//envoie du packet, attente de l'acquittement, 
		// réessaie si il y problème avec au max MAX_NUMBER_OF_ATTEMPT essais
		l_uc_Attempt=0;
		
		// Ajout du debut de paquet (SOP)
		s_SPRS232_MakeSOP(SPRS232_ID_ID_PACKET_DATA,	// packet type
						(UC)(l_ul_Count==0?TRUE:FALSE),	// FIRST
						(UC)((i_ul_DataSize-l_ul_Count-l_ul_SizeOfSendData)==0?TRUE:FALSE), // LAST
						gs_uc_RequestCounter,
						l_auc_PacketToSend,		// pointeur sur le debut du paquet
						&l_ul_PacketCurrentSize);	// position ou ecrire dans le paquet
		
		// Ajout des data et du CRC, realisation du stuffing 
		s_SPRS232_AddDataToPacket(l_auc_PacketToSend,&l_ul_PacketCurrentSize,
								  i_puc_Data+l_ul_Count,l_ul_SizeOfSendData);
		
		// Ajout de la fin de paquet (EOP)
		s_SPRS232_AddEOP(l_auc_PacketToSend,&l_ul_PacketCurrentSize);

		do
		{	
			l_nStatus =	RS232_Write(
										l_auc_PacketToSend,
										l_ul_PacketCurrentSize,
										NULL);
			if(l_nStatus)
				return l_nStatus;
		
			//attente de l'acquittement du paquet envoyé
			//tant que la réception du SOP ne sort pas en erreur 
			do		
			{
				l_nStatus =	s_SPRS232_ReceiveSOP(&l_ucID,&l_bFirst,&l_bLast,
												 &l_uc_RC,ACK_TIMEOUT);		

			}while ( (!l_nStatus)&&(gs_uc_RequestCounter!=l_uc_RC));

			l_uc_Attempt++; 
		
			if(l_nStatus != SPRS232_OK)
			{
				continue;
			} 
			else if (l_ucID==SPRS232_ID_ID_PACKET_ACK)
			{
				gs_uc_RequestCounter++;				
				break;
			}
			else if(l_ucID==SPRS232_ID_ID_PACKET_NACK)  
			{
				// Si on recoit un Nack, on augmente a 5 le nombre
				// de tentative d'envoi du paquet. 
				l_uc_MaxNumberOfAttempt = 5; 
				continue;
			}
			else
			{
				continue;
			}

		}while (l_uc_Attempt < l_uc_MaxNumberOfAttempt);
		
		if (l_uc_Attempt >= l_uc_MaxNumberOfAttempt)
		{
			gs_uc_RequestCounter++;
			return l_nStatus;
		}
		
		l_ul_Count += l_ul_SizeOfSendData;// nb octets envoyés
		
	}while (i_ul_DataSize-l_ul_Count >0);

	return SPRS232_OK;
}

I SPRS232_ReceiveFreeEx
(
	MORPHO_HANDLE	i_h_Handle,
	PUC*		io_ppuc_Data
)
{
	if ( *io_ppuc_Data != NULL)
	{
		free(*io_ppuc_Data);
		*io_ppuc_Data = NULL;
	}

	return SPRS232_OK;
}

I SPRS232_ReceiveEx
(
	MORPHO_HANDLE	i_h_Handle,
	UL		i_ul_Timeout,
	PUC*	io_ppuc_Data,
	PUL		o_pul_DataSize
)
{
	*io_ppuc_Data = malloc(SPRS232_RECEIVE_ALLOC);

	return SPRS232_Receive(	i_h_Handle,
				*io_ppuc_Data,		//UC * i_puc_Buffer, 
				SPRS232_RECEIVE_ALLOC,	//UL i_ul_BufferSize, 
				o_pul_DataSize,		//UL *o_pul_ILVSize,
				i_ul_Timeout		//DWORD i_dw_ReadTotalTimeoutConstant)
			);
}
//+---------------------------------------------------------------------------
//
//  Function:   SPRS232_Receive implementation
//
//  Synopsis: This fonction receive the packet
//							
//
//  Arguments:	UC * i_puc_Buffer	: data to return
//				UL i_ul_BufferSize	: size of buffer
//				UL *o_pul_ILVSize	: size of the ILV data
//				DWORD i_dw_ReadTotalTimeoutConstant
//
//
//	Returned value :	return SPRS232_OK	
//						return SPRS232_ERR_TIMEOUT
//						
//
//  History:
//
//  Notes:	
//	
//----------------------------------------------------------------------------
I SPRS232_Receive(
	MORPHO_HANDLE	i_h_Handle,
	UC * i_puc_Buffer, 
	UL i_ul_BufferSize, 
	UL *o_pul_ILVSize,
	DWORD i_dw_ReadTotalTimeoutConstant
)
{
	UC	l_uc_Id;
	I	l_nStatus;
	I	l_nStatus2;
	UL	l_ul_PacketSize;
	BOOLEAN l_b_Last;
	UC	l_uc_First;
	UC	l_uc_RC;
	US  l_us_ACK_RC;
	UC l_auc_PacketToSend[4]; // paquet pour ACK ou NACK (3 octets + 1 si stuffing sur RC))
	UL l_ul_PacketCurrentSize;
	BOOLEAN l_b_Start;
	UL		l_ul_BufferSize;

////initialisation des variables
	l_b_Last = FALSE;
	l_us_ACK_RC =  0x100;	// cas impossible
	l_b_Start = FALSE;
////END

	if(!g_b_SPRS232IsInit)
		return SPRS232_ERR_INIT;

	do
	{
		l_ul_BufferSize = i_ul_BufferSize - *o_pul_ILVSize;

		do
		{
			// receive SOP data avec un timeout donné en parametre
			l_nStatus =	s_SPRS232_ReceiveSOP(
												&l_uc_Id,
												&l_uc_First,
												&l_b_Last,
												&l_uc_RC,
												MAX(i_dw_ReadTotalTimeoutConstant,SPRS232_TIMEOUT_ACK + SPRS232_TIMEOUT_INTERVAL));
							
			if(l_nStatus != SPRS232_OK)
			{
				return l_nStatus;
			}

			if( ( l_uc_First == SPRS232_ID_TYP_PACKET_FIRST ) &&
				!(l_us_ACK_RC == l_uc_RC)	)	// On vérifie qu'on ne l'a pas déjà acquitté...
			{
				*o_pul_ILVSize = 0;
				l_b_Start = TRUE;
			}
		}while ((l_uc_Id!=SPRS232_ID_ID_PACKET_DATA) || (l_b_Start == FALSE));

		// receive the packet data
		l_nStatus =	s_SPRS232_ReceivePacket(i_puc_Buffer+*o_pul_ILVSize,
											&l_ul_PacketSize,
											l_ul_BufferSize);

		if (l_nStatus==SPRS232_OK || l_nStatus==SPRS232_ERR_SMALL_BUF)
		{
			if( (l_us_ACK_RC == l_uc_RC) && !( (l_uc_First != 0) && (l_b_Last !=0 ) ) )
				continue;

			// Send ACK
			s_SPRS232_MakeSOP(SPRS232_ID_ID_PACKET_ACK,TRUE,TRUE,l_uc_RC,
							  l_auc_PacketToSend,&l_ul_PacketCurrentSize);
				
			l_nStatus2 = RS232_Write(
										l_auc_PacketToSend,
										l_ul_PacketCurrentSize,
										NULL);
			if(l_nStatus2 != SPRS232_OK)
			{
				return l_nStatus;
			}

			l_us_ACK_RC = l_uc_RC; // dernier paquet acquitte
			*o_pul_ILVSize+=l_ul_PacketSize;	
			
			if(l_nStatus==SPRS232_ERR_SMALL_BUF)
				return l_nStatus;
		}		
		else /*if(l_nStatus!=RS232ERR_READ_TIMEOUT)*/
		{
			RS232_Clear();

			// Verifie qu'un ACK n'a pas deja ete envoyé pour ce paquet (vu sous WinCE...)
			if( (l_us_ACK_RC == l_uc_RC) )
				continue;

			// Send NACK
			s_SPRS232_MakeSOP(SPRS232_ID_ID_PACKET_NACK, TRUE, TRUE, l_uc_RC,
							  l_auc_PacketToSend, &l_ul_PacketCurrentSize);
					
			l_nStatus =	RS232_Write(
										l_auc_PacketToSend,
										l_ul_PacketCurrentSize,
										NULL);
			if(l_nStatus != SPRS232_OK)
			{
				return l_nStatus;
			}
			l_b_Last = FALSE;
		}
		
	}while (!l_b_Last);

	return SPRS232_OK;
}

/*******************************************************************************
-- RS232_Break implementation
*******************************************************************************/
I SPRS232_Break
(
	MORPHO_HANDLE	i_h_Handle,
	UL i_ul_Time
)
{
	if(!g_b_SPRS232IsInit)
		return SPRS232_ERR_INIT;

	return RS232_Break(i_ul_Time);
}

/*******************************************************************************
description:

*******************************************************************************/
I SPRS232_Write(
	MORPHO_HANDLE	i_h_Handle,
	UC *i_puc_Buf,
	UL i_ul_Size,
	UL *o_pul_BytesWritten
)
{
	if(!g_b_SPRS232IsInit)
		return SPRS232_ERR_INIT;

	return RS232_Write(
				   i_puc_Buf,
				   i_ul_Size,
				   o_pul_BytesWritten);
}

/*******************************************************************************
description:

*******************************************************************************/
I SPRS232_Read
(
	MORPHO_HANDLE	i_h_Handle,
	PUC	i_puc_Buf,
	UL	i_ul_Size,
	UL	i_ul_ReadTotalTimeoutConstant
)
{
	if(!g_b_SPRS232IsInit)
		return SPRS232_ERR_INIT;

	return RS232_Read(
				   i_puc_Buf,
				   i_ul_Size,
				   i_ul_ReadTotalTimeoutConstant);
}

/*******************************************************************************
description:

*******************************************************************************/
I SPRS232_GetConfig
(
	MORPHO_HANDLE	i_h_Handle
)
{
	if(!g_b_SPRS232IsInit)
		return SPRS232_ERR_INIT;

	return RS232_GetConfig();

}


