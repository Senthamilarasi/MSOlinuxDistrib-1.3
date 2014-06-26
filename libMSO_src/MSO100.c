#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pthread.h>
#include <signal.h>
#include <semaphore.h>

#include "libMSO.h"
#include "MSO100.h"
#include "SpIlv.h"
#include "sp_rs232.h"
#include "Sp_Usb.h"

HANDLE g_h_Sprs232Handle;

pthread_mutex_t 	g_h_MutexFrame = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t 	g_h_MutexILV = PTHREAD_MUTEX_INITIALIZER;

#define LOCK(mutex) 	pthread_mutex_lock(&mutex)
#define UNLOCK(mutex)	pthread_mutex_unlock(&mutex)


/////////////////////////////////////////////////////////////////////
//	RS232							   //
/////////////////////////////////////////////////////////////////////
I MSO_NegociateBaudRate
(
	MORPHO_HANDLE	*i_ph_Mso100Handle,
	PC		i_pc_String,
	UL		i_ul_Param,
	PVOID	io_pv_Param
)
{
	I	l_i_Ret;
	PC	l_str_comName;
	I	l_i_baudRate;
	UC	l_uc_Status;
	PT_MSO100_STRUCT	l_px_Mso100Struct;

	if(i_ph_Mso100Handle == NULL)
	{
		return MSO_BAD_PARAMETER;
	}

	l_str_comName	= i_pc_String;
	l_i_baudRate	= i_ul_Param;

	l_i_Ret = SPRS232_Initialize(i_ph_Mso100Handle,l_str_comName,9600);

	l_px_Mso100Struct = ((PT_MSO100_STRUCT)g_h_Sprs232Handle);
	l_px_Mso100Struct->m_b_Initialised = TRUE;

	//negotiate baud rate
	if (l_i_Ret == RETURN_NO_ERROR)
		MSO_Break( *i_ph_Mso100Handle, 50);

	if (l_i_baudRate!=9600)
	{
		//config uart param values are defined in ILV interface specification
		if (l_i_Ret == RETURN_NO_ERROR)
			l_i_Ret = MSO_ConfigUart(
						l_px_Mso100Struct,
						l_i_baudRate,		//i_ul_BaudRate
						8,	//i_uc_ByteSize
						1,	//i_uc_StopBits
						0,	//i_uc_Parity
						2,	//i_uc_FlowCtrl = software flow control
						0,	//not used
						0,	//not used
						&l_uc_Status);

		if( (l_i_Ret == RETURN_NO_ERROR) && (l_uc_Status != ILV_OK))
			l_i_Ret = STATUS_ERR_REF + l_uc_Status;

		if (l_i_Ret == RETURN_NO_ERROR)
			l_i_Ret = SPRS232_SetBaudRate(*i_ph_Mso100Handle,l_i_baudRate);
	}

	if(l_i_Ret)
		SPRS232_Close(i_ph_Mso100Handle);

	return l_i_Ret;
}

/////////////////////////////////////////////////////////////////////
I MSO_Break
(
	MORPHO_HANDLE	i_h_Mso100Handle,
	UL i_ul_Time
)
{
	return SPRS232_Break(i_h_Mso100Handle,i_ul_Time);
}


/////////////////////////////////////////////////////////////////////
//	Open, Close, Read, Write				   //
/////////////////////////////////////////////////////////////////////
I MSO_ComOpen
(
	MORPHO_HANDLE				*o_ph_Mso100Handle,
	T_MORPHO_CALLBACK_COM_OPEN		i_pf_CallbackComOpen,
	T_MORPHO_CALLBACK_COM_SEND		i_pf_CallbackComSend,
	T_MORPHO_CALLBACK_COM_RECEIVE		i_pf_CallbackComReceive,
	T_MORPHO_CALLBACK_COM_RECEIVE_FREE	i_pf_CallbackComReceiveFree,
	T_MORPHO_CALLBACK_COM_CLOSE		i_pf_CallbackComClose,
	PC					i_pc_String,
	UL					i_ul_Param,
	PVOID					io_pv_param
)
{
	I			l_i_Ret;
	PT_MSO100_STRUCT	l_px_Mso100Struct;

	*o_ph_Mso100Handle = NULL;

	l_px_Mso100Struct = malloc ( sizeof ( T_MSO100_STRUCT ) );
	if ( l_px_Mso100Struct == NULL )
		return MSO_ERROR_MEMORY;
	memset ( l_px_Mso100Struct, 0, sizeof ( T_MSO100_STRUCT ) );

	if(!i_pf_CallbackComOpen)
		return MSO_COM_OPEN_BAD_PARAMETER;
	if(!i_pf_CallbackComSend)
		return MSO_COM_OPEN_BAD_PARAMETER;
	if(!i_pf_CallbackComReceive)
		return MSO_COM_OPEN_BAD_PARAMETER;
	if(!i_pf_CallbackComReceiveFree)
		return MSO_COM_OPEN_BAD_PARAMETER;
	if(!i_pf_CallbackComClose)
		return MSO_COM_OPEN_BAD_PARAMETER;

	l_px_Mso100Struct->g_i_Stop = 0;
	
	pthread_mutex_init(&g_h_MutexFrame, NULL);
	pthread_mutex_init(&g_h_MutexILV, NULL);

	l_px_Mso100Struct->g_pf_CallbackComOpen		= i_pf_CallbackComOpen;
	l_px_Mso100Struct->g_pf_CallbackComSend		= i_pf_CallbackComSend;
	l_px_Mso100Struct->g_pf_CallbackComReceive	= i_pf_CallbackComReceive;
	l_px_Mso100Struct->g_pf_CallbackComReceiveFree  = i_pf_CallbackComReceiveFree;
	l_px_Mso100Struct->g_pf_CallbackComClose	= i_pf_CallbackComClose;

	LOCK(g_h_MutexFrame);

	g_h_Sprs232Handle = l_px_Mso100Struct;

	l_i_Ret = l_px_Mso100Struct->g_pf_CallbackComOpen(
									&l_px_Mso100Struct->m_h_Handle,
									i_pc_String,
									i_ul_Param,
									io_pv_param
									);

	UNLOCK(g_h_MutexFrame);

	l_px_Mso100Struct->m_b_Initialised = TRUE;

	if(io_pv_param != NULL)
	{
		strncpy(l_px_Mso100Struct->m_str_Interface, io_pv_param, sizeof(l_px_Mso100Struct->m_str_Interface));
	}

	*o_ph_Mso100Handle = l_px_Mso100Struct;

	return l_i_Ret;
}

/////////////////////////////////////////////////////////////////////
I MSO_ComSend
(
	MORPHO_HANDLE	i_h_Mso100Handle,
	UL		i_ul_Timeout,
	PUC		i_puc_Data,
	UL		i_ul_DataSize
)
{
	I	l_i_Ret;
	PUC	l_puc_ILV;
	PUC	l_puc_Data;
	UL	l_ul_DataSize;

	PT_MSO100_STRUCT	l_px_Mso100Struct;

	if(i_h_Mso100Handle == NULL)
	{
	    return MSO_BAD_PARAMETER;
	}

	l_px_Mso100Struct = ((PT_MSO100_STRUCT)i_h_Mso100Handle);

	if( l_px_Mso100Struct->m_b_Initialised == FALSE )
	{
		return COM_ERR_NOT_OPEN;
	}

	if(!i_puc_Data)
		return MSO_BAD_PARAMETER;

	l_puc_ILV = NULL;

	l_puc_Data	= i_puc_Data;
	l_ul_DataSize	= i_ul_DataSize;
	l_i_Ret = RETURN_NO_ERROR;

	if(l_i_Ret == RETURN_NO_ERROR)
	{
		LOCK(g_h_MutexFrame);
		l_i_Ret = l_px_Mso100Struct->g_pf_CallbackComSend(
										l_px_Mso100Struct->m_h_Handle,
										i_ul_Timeout,
										l_puc_Data,
										l_ul_DataSize
										);
		UNLOCK(g_h_MutexFrame);
	}

	if(l_puc_ILV)
		free(l_puc_ILV);

	return l_i_Ret;
}

/////////////////////////////////////////////////////////////////////
I MSO_ComReceive
(
	MORPHO_HANDLE	i_h_Mso100Handle,
	UL		i_ul_Timeout,
	PUC*		io_ppuc_Data,
	PUL		o_pul_DataSize
)
{
	I			l_i_Ret;
	PT_MSO100_STRUCT	l_px_Mso100Struct;

	if(i_h_Mso100Handle == NULL)
	{
	    return MSO_BAD_PARAMETER;
	}

	l_px_Mso100Struct = ((PT_MSO100_STRUCT)i_h_Mso100Handle);

	if( l_px_Mso100Struct->m_b_Initialised == FALSE )
	{
		return COM_ERR_NOT_OPEN;
	}

	l_i_Ret = l_px_Mso100Struct->g_pf_CallbackComReceive( l_px_Mso100Struct->m_h_Handle,
							      i_ul_Timeout,
							      io_ppuc_Data,
							      o_pul_DataSize
							    );
	if(l_i_Ret)
	{
		*o_pul_DataSize = 0;
		MSO_FrameRelease(i_h_Mso100Handle,io_ppuc_Data);
	}

	return l_i_Ret;
}

/////////////////////////////////////////////////////////////////////
I MSO_ComReceiveFree
(
	MORPHO_HANDLE	i_h_Mso100Handle,
	PUC*		io_ppuc_Data
)
{
	I			l_i_Ret;
	PT_MSO100_STRUCT	l_px_Mso100Struct;

	if(i_h_Mso100Handle == NULL)
	{
	    return MSO_BAD_PARAMETER;
	}

	l_px_Mso100Struct = ((PT_MSO100_STRUCT)i_h_Mso100Handle);

	if(l_px_Mso100Struct->g_pf_CallbackComReceiveFree)
	{
		l_i_Ret = l_px_Mso100Struct->g_pf_CallbackComReceiveFree(
							l_px_Mso100Struct->m_h_Handle,
							io_ppuc_Data
							);
	}
	else
	{
		if(*io_ppuc_Data == NULL)
			return RETURN_NO_ERROR;
		else
			return COM_ERR_NOT_OPEN;
	}
	return l_i_Ret;
}

/////////////////////////////////////////////////////////////////////
I MSO_InitCom
(
	MORPHO_HANDLE	*o_ph_Mso100Handle,
	PC		i_str_Interface,
	PC		i_str_comName,
	I		i_i_baudRate
)
{
	I				l_i_Ret;
	PT_MSO_USB_DEVICE_PROPERTIES	l_ppx_DeviceProperties;
	unsigned long			l_pul_NbreDevices;
	BOOL				l_b_UnknownName;
	int				l_i_Index;
	PC				l_str_comName;

	l_b_UnknownName = FALSE;
	l_str_comName = NULL;

	if (strcmp(i_str_Interface, COM_USB) == 0)
	{
		l_i_Ret = RETURN_NO_ERROR;

		if ( (i_str_comName == NULL) || (strlen(i_str_comName) == 0)
					|| (strnicmp(i_str_comName, "/proc/bus/usb/", 14 ) == 0) )
		{
			l_i_Ret = MSO_Usb_EnumDevices(&l_ppx_DeviceProperties, &l_pul_NbreDevices);

			if(l_i_Ret == RETURN_NO_ERROR)
			{
				if(l_pul_NbreDevices == 0)
				{
					MSO_Usb_ReleaseEnumDevices(&l_ppx_DeviceProperties, l_pul_NbreDevices);
					return SPUSB_DEVICE_NOT_PRESENT;
				}

				l_b_UnknownName = TRUE;

				for ( l_i_Index = 0; l_i_Index<(int)l_pul_NbreDevices; l_i_Index++ )
				{
					// On prend le premier MSO arriv�
					if ( l_ppx_DeviceProperties == NULL )
					{
						continue;
					}
					if ( (i_str_comName == NULL) || (strlen(i_str_comName) == 0) )
					{
						l_str_comName = (PC) (l_ppx_DeviceProperties+l_i_Index)->m_puc_SerialNumber;
						break;
					}
					if ( stricmp ( i_str_comName, (PC) (l_ppx_DeviceProperties+l_i_Index)->m_puc_DevicePath ) == 0)
					{
						l_str_comName = (PC) (l_ppx_DeviceProperties+l_i_Index)->m_puc_SerialNumber;
						break;
					}
				}
			}
		}
		else
		{
			l_str_comName = i_str_comName;
		}

		// Si il est toujours NULL
		if ( l_str_comName == NULL )
		{
			return SPUSB_DEVICE_NOT_PRESENT;
		}

		if(l_i_Ret == RETURN_NO_ERROR)
		{
			l_i_Ret = MSO_ComOpen ( o_ph_Mso100Handle,
						SpUsb_OpenEx,	 //T_MORPHO_CALLBACK_COM_OPEN	i_pf_CallbackComOpen,
						SpUsb_WriteFrame,//T_MORPHO_CALLBACK_COM_SEND	i_pf_CallbackComSend,
						SpUsb_ReadFrame, //T_MORPHO_CALLBACK_COM_RECEIVE i_pf_CallbackComReceive,
						SpUsb_ReadFrameReleaseEx, //T_MORPHO_CALLBACK_COM_RECEIVE_FREE	i_pf_CallbackComReceiveFree,
						SpUsb_Close,	 //T_MORPHO_CALLBACK_COM_CLOSE	i_pf_CallbackComClose
						l_str_comName,	 //PC    i_pc_String,
						(UL)i_i_baudRate,//UL	 i_ul_Param,
						i_str_Interface	 //PVOID io_pv_param
					      );
			if(l_b_UnknownName == TRUE)
			{
				l_i_Ret = MSO_Usb_ReleaseEnumDevices(&l_ppx_DeviceProperties, l_pul_NbreDevices);
			}
		}
	}
	else
	{

		l_i_Ret = MSO_ComOpen (
					o_ph_Mso100Handle,
					MSO_NegociateBaudRate,	//T_MORPHO_CALLBACK_COM_OPEN	i_pf_CallbackComOpen,
					SPRS232_SendEx,		//T_MORPHO_CALLBACK_COM_SEND	i_pf_CallbackComSend,
					SPRS232_ReceiveEx,	//T_MORPHO_CALLBACK_COM_RECEIVE i_pf_CallbackComReceive,
					SPRS232_ReceiveFreeEx,	//T_MORPHO_CALLBACK_COM_RECEIVE_FREE i_pf_CallbackComReceiveFree,
					SPRS232_Close,		//T_MORPHO_CALLBACK_COM_CLOSE	i_pf_CallbackComClose
					i_str_comName,		//PC				i_pc_String,
					(UL)i_i_baudRate,	//UL				i_ul_Param,
					i_str_Interface		//PVOID				io_pv_param
					);

	}

	if( (*o_ph_Mso100Handle != NULL) && (i_str_Interface != NULL) && (strlen(i_str_Interface) !=0) )
	{
		strncpy ( ((PT_MSO100_STRUCT)*o_ph_Mso100Handle)->m_str_Interface, i_str_Interface, 
					sizeof ( ((PT_MSO100_STRUCT)*o_ph_Mso100Handle)->m_str_Interface ) );
	}

	if(l_i_Ret)
	{
		MSO_CloseCom(o_ph_Mso100Handle);
	}

	return l_i_Ret;
}

/////////////////////////////////////////////////////////////////////
I MSO_CloseCom
(
	MORPHO_HANDLE	*io_ph_Mso100Handle
)
{
	I			l_i_Ret;

	PT_MSO100_STRUCT	l_px_mso100_struct;

	if(*io_ph_Mso100Handle == NULL)
	{
		l_i_Ret = MSO_BAD_PARAMETER;
	}
	else
	{
		l_px_mso100_struct = ((PT_MSO100_STRUCT)*io_ph_Mso100Handle);
		
		LOCK(g_h_MutexILV);
		LOCK(g_h_MutexFrame);

		l_px_mso100_struct->g_i_Stop = 1;

		pthread_mutex_destroy(&g_h_MutexFrame);
		pthread_mutex_destroy(&g_h_MutexILV);

		l_i_Ret = l_px_mso100_struct->g_pf_CallbackComClose (&l_px_mso100_struct->m_h_Handle);

		l_px_mso100_struct->g_pf_CallbackComOpen		= NULL;
		l_px_mso100_struct->g_pf_CallbackComSend		= NULL;
		l_px_mso100_struct->g_pf_CallbackComReceive		= NULL;
		l_px_mso100_struct->g_pf_CallbackComReceiveFree 	= NULL;
		l_px_mso100_struct->g_pf_CallbackComClose		= NULL;

		free ( l_px_mso100_struct );
		l_px_mso100_struct = NULL;

		*io_ph_Mso100Handle = NULL;
	}

	return l_i_Ret;
}

/*******************************************************************************
description :
  o_ppuc_Data can be null
  o_puc_ILV_Status can be null
  o_pi_EmbeddedError can be null
*******************************************************************************/
I MSO_SendReceive
(
	MORPHO_HANDLE	i_h_Mso100Handle,
	US		i_us_I,			// Identifier
	PUC		i_puc_Frame,		// data to send
	UL		i_ul_len,		// number of bytes to write
	PUC		*o_ppuc_dataOut,	// response buffer
	PUL		o_pul_len,		// response length
	PUC		*o_ppuc_Data,		// ILV data
	PUC		o_puc_ILV_Status,
	PI		o_pi_EmbeddedError
)
{
	if(i_h_Mso100Handle == NULL)
	{
		return MSO_BAD_PARAMETER;
	}

	return MSO_SendReceiveTimeout(  i_h_Mso100Handle,
					WRITE_TIMEOUT,
					READ_TIMEOUT,
					i_us_I,
					i_puc_Frame,
					i_ul_len,
					o_ppuc_dataOut,
					o_pul_len,
					o_ppuc_Data,
					o_puc_ILV_Status,
					o_pi_EmbeddedError
				     );
}

/*******************************************************************************
description :
  o_ppuc_Data can be null
  o_puc_ILV_Status can be null
  o_pi_EmbeddedError can be null
*******************************************************************************/
I MSO_SendReceiveTimeout
(
	MORPHO_HANDLE	i_h_Mso100Handle,
	UL		i_ul_TransmitTimeout,	// reception timeout, only for USB (ms)
	UL		i_ul_ReceiveTimeout,	// reception timeout (ms)
	US		i_us_I,			// Identifier
	PUC		i_puc_Frame,		// data to send
	UL		i_ul_len,		// number of bytes to write
	PUC		*o_ppuc_dataOut,	// response buffer
	PUL		o_pul_len,		// response length
	PUC		*o_ppuc_Data,		// ILV data
	PUC		o_puc_ILV_Status,
	PI		o_pi_EmbeddedError
)
{
	I	l_i_Ret;
	US	l_us_I;
	I	l_i_AsyncILV;
	UL	l_ul_Timeout;
	UL	l_ul_ReceiveTimeout;
	PUC	l_puc_ReceiveV;
	I	l_i_EmbeddedError;

	PT_MSO100_STRUCT	l_px_Mso100Struct;

	if(i_h_Mso100Handle == NULL)
	{
	    return MSO_BAD_PARAMETER;
	}

	l_px_Mso100Struct = ((PT_MSO100_STRUCT)i_h_Mso100Handle);

	l_i_EmbeddedError = 0;
	l_i_AsyncILV = 0;
	l_ul_Timeout = 0;
	l_ul_ReceiveTimeout = 0;
	l_puc_ReceiveV = NULL;

	l_i_Ret = RETURN_NO_ERROR;

	*o_ppuc_dataOut = NULL;
	*o_pul_len = 0;
	if(o_ppuc_Data)
		*o_ppuc_Data = NULL;
	if(o_puc_ILV_Status)
		*o_puc_ILV_Status = ILV_OK;
	if(o_pi_EmbeddedError)
		*o_pi_EmbeddedError = RETURN_NO_ERROR;

	l_us_I = 0;

	if( l_px_Mso100Struct->m_b_Initialised == FALSE )
	{
		return COM_ERR_NOT_OPEN;
	}

	LOCK(g_h_MutexILV);
	
	l_i_Ret = MSO_ComSend(  i_h_Mso100Handle,
				i_ul_TransmitTimeout,	//UL		i_ul_Timeout,
				i_puc_Frame,		//PUC		i_puc_Data,
				i_ul_len		//UL		i_ul_DataSize
			     );
	if(l_i_Ret)
	{
		UNLOCK(g_h_MutexILV);
		return l_i_Ret;
	}

	l_ul_ReceiveTimeout = i_ul_ReceiveTimeout;

	while (l_i_Ret == RETURN_NO_ERROR)
	{
		if (strcmp(l_px_Mso100Struct->m_str_Interface, COM_USB) != 0)
		{
			// lecture de la reponse. La tempo est courte pour permettre a un message Cancel de passer
			if(l_ul_ReceiveTimeout >= TEMPO_RECEIVE)
				l_ul_Timeout = TEMPO_RECEIVE;
			else
				l_ul_Timeout = l_ul_ReceiveTimeout;
		}
		else
			l_ul_Timeout = l_ul_ReceiveTimeout;

		l_i_Ret = MSO_ComReceive ( i_h_Mso100Handle,
					   l_ul_Timeout,	//UL		i_ul_Timeout,
					   o_ppuc_dataOut,	//PUC*		io_ppuc_Data,
					   o_pul_len		//PUL		o_pul_DataSize
					 );

		if ( l_i_Ret == RETURN_NO_ERROR )
		{
			if(*o_ppuc_dataOut==NULL || *o_pul_len == 0)
			{
				l_i_Ret = COM_ERR;
			}
		}
	
		if ( l_i_Ret == RETURN_NO_ERROR )
		{
			l_us_I = ILV_GetI(*o_ppuc_dataOut);

			if(l_us_I == ILV_INVALID)
			{
				l_i_Ret = COM_RECEIVE_ILV_INVALID;
				break;
			}
			else
			{
				// we check if this event has been registered to a Callback
				l_i_AsyncILV = MSO_RunAsyncILV(	i_h_Mso100Handle, l_us_I, *o_ppuc_dataOut );
				if ( l_i_AsyncILV != 0)
					break;
			}
		}
		MSO_FrameRelease(i_h_Mso100Handle, o_ppuc_dataOut); 

		if ( (l_i_Ret == RS232ERR_READ_TIMEOUT) || (l_i_Ret == SPUSB_ERR_TIMEOUT) 
							|| (l_i_Ret == COMERR_TIMEOUT_RECEIVE) )
		{
			l_ul_ReceiveTimeout -= l_ul_Timeout;

			if(l_px_Mso100Struct->g_i_Stop)
				l_i_Ret = COM_STOP;
			else if (l_ul_ReceiveTimeout != 0)
				l_i_Ret = RETURN_NO_ERROR;
		}
	}

	if (l_i_Ret == RETURN_NO_ERROR)
		l_i_Ret = ILV_Check(*o_ppuc_dataOut,*o_pul_len);

	if ( (l_i_Ret == RETURN_NO_ERROR) && (l_us_I != i_us_I) )
		l_i_Ret = COM_ERROR_SYNCHRO_I;

	if(l_i_Ret == RETURN_NO_ERROR)
	{
		l_puc_ReceiveV = ILV_GetV(*o_ppuc_dataOut);

		l_i_Ret = ILV_GetErrorILV( l_puc_ReceiveV,		//PUC	i_puc_V,
					   (PC)o_puc_ILV_Status,	//PC	o_pc_ILV_Status,
				 	   &l_i_EmbeddedError);		//PI	o_pi_EmbeddedError);
	}
	if(l_i_Ret == RETURN_NO_ERROR)
	{
		if (l_i_EmbeddedError)
			l_px_Mso100Struct->g_i_EmbeddedError = l_i_EmbeddedError;

		if(o_pi_EmbeddedError)
			*o_pi_EmbeddedError = l_i_EmbeddedError;

		if(o_ppuc_Data)
			*o_ppuc_Data = l_puc_ReceiveV + sizeof(UC);
	}

	if(l_i_Ret || l_i_EmbeddedError)
	{
		*o_pul_len = 0;
		MSO_FrameRelease(i_h_Mso100Handle, o_ppuc_dataOut);
	}

	UNLOCK(g_h_MutexILV);

	return l_i_Ret;
}

/////////////////////////////////////////////////////////////////////
void MSO_FrameRelease
(
	MORPHO_HANDLE	i_h_Mso100Handle,
	PUC		*io_ppuc_Data
)
{
	if(i_h_Mso100Handle != NULL)
	{
		MSO_ComReceiveFree (i_h_Mso100Handle, io_ppuc_Data);
	}
}

/////////////////////////////////////////////////////////////////////
I MSO_RegisterAsyncILV
(
	MORPHO_HANDLE		i_h_Mso100Handle,
	I			i_us_I,
	T_pFuncILV_Buffer	i_p_Callback,
	PVOID			i_pv_Context
)
{
	PT_MSO100_STRUCT	l_px_Mso100Struct;

	if(i_h_Mso100Handle == NULL)
	{
	    return MSO_BAD_PARAMETER;
	}

	l_px_Mso100Struct = ((PT_MSO100_STRUCT)i_h_Mso100Handle);

	if (l_px_Mso100Struct->g_i_nb_RecordedILV < NB_RECORDED_ILV)
	{
		l_px_Mso100Struct->g_px_IlvTable[l_px_Mso100Struct->g_i_nb_RecordedILV].m_us_Ident = i_us_I;
		l_px_Mso100Struct->g_px_IlvTable[l_px_Mso100Struct->g_i_nb_RecordedILV].m_x_Callback = i_p_Callback;
		l_px_Mso100Struct->g_px_IlvTable[l_px_Mso100Struct->g_i_nb_RecordedILV].m_pv_Context = i_pv_Context;

		l_px_Mso100Struct->g_i_nb_RecordedILV++;
	}

	return RETURN_NO_ERROR;
}

/////////////////////////////////////////////////////////////////////
I MSO_UnregisterAsyncILV
(
	MORPHO_HANDLE	i_h_Mso100Handle,
	I		i_us_I
)
{
	I			l_i_Cnt, l_i_Cnt2;
	PT_MSO100_STRUCT	l_px_Mso100Struct;

	if(i_h_Mso100Handle == NULL)
	{
	    return MSO_BAD_PARAMETER;
	}

	l_px_Mso100Struct = ((PT_MSO100_STRUCT)i_h_Mso100Handle);
	LOCK(g_h_MutexFrame);
	for (l_i_Cnt=0; l_i_Cnt<l_px_Mso100Struct->g_i_nb_RecordedILV; l_i_Cnt++)
	{
		if (l_px_Mso100Struct->g_px_IlvTable[l_i_Cnt].m_us_Ident == i_us_I)
		{
			for (l_i_Cnt2=l_i_Cnt; l_i_Cnt2<l_px_Mso100Struct->g_i_nb_RecordedILV-1; l_i_Cnt2++)
			{
				l_px_Mso100Struct->g_px_IlvTable[l_i_Cnt2].m_us_Ident   = l_px_Mso100Struct->g_px_IlvTable[l_i_Cnt2+1].m_us_Ident;
				l_px_Mso100Struct->g_px_IlvTable[l_i_Cnt2].m_x_Callback = l_px_Mso100Struct->g_px_IlvTable[l_i_Cnt2+1].m_x_Callback;
				l_px_Mso100Struct->g_px_IlvTable[l_i_Cnt2].m_pv_Context = l_px_Mso100Struct->g_px_IlvTable[l_i_Cnt2+1].m_pv_Context;
			}
			l_px_Mso100Struct->g_i_nb_RecordedILV--;
			break;
		}
	}

	UNLOCK(g_h_MutexFrame);

	return RETURN_NO_ERROR;
}

/////////////////////////////////////////////////////////////////////
I MSO_UnregisterAllAsyncILV
(
	MORPHO_HANDLE	i_h_Mso100Handle
)
{
	PT_MSO100_STRUCT	l_px_Mso100Struct;

	if(i_h_Mso100Handle == NULL)
	{
	    return MSO_BAD_PARAMETER;
	}

	l_px_Mso100Struct = ((PT_MSO100_STRUCT)i_h_Mso100Handle);

	LOCK(g_h_MutexFrame);
	l_px_Mso100Struct->g_i_nb_RecordedILV = 0;
	UNLOCK(g_h_MutexFrame);

	return RETURN_NO_ERROR;
}

/////////////////////////////////////////////////////////////////////
I MSO_RunAsyncILV
(
	MORPHO_HANDLE		i_h_Mso100Handle,
	US			i_us_Ident,		// Identifier
	PUC			i_puc_ILV_ReceiveBuffer
)
{
	I		l_i_Ret = -1;
	I		l_i_Cnt;
	UC		l_uc_ILV_Status, l_uc_ILV_Type;
	PUC		l_puc_V;
	PUC		l_puc_Param;
	UL		l_ul_SizeParam;

	PT_MSO100_STRUCT	l_px_Mso100Struct;

	if(i_h_Mso100Handle == NULL)
	{
	    return MSO_BAD_PARAMETER;
	}

	l_px_Mso100Struct = ((PT_MSO100_STRUCT)i_h_Mso100Handle);

	if ( (l_px_Mso100Struct->g_i_nb_RecordedILV == 0) && (i_us_Ident == ILV_ASYNC_MESSAGE) )
		return RETURN_NO_ERROR;	// remaining asynchronous message

	for (l_i_Cnt=0; l_i_Cnt<l_px_Mso100Struct->g_i_nb_RecordedILV; l_i_Cnt++)
	{
		l_puc_V = ILV_GetV(i_puc_ILV_ReceiveBuffer);
		l_uc_ILV_Status = *l_puc_V;
		l_puc_V+=sizeof(l_uc_ILV_Status);

		// lecture du sous ILV
		l_uc_ILV_Type = *l_puc_V;

		if (i_us_Ident == ILV_ASYNC_MESSAGE)
		{
			l_ul_SizeParam = ILV_GetL(l_puc_V);

			l_puc_V = ILV_GetV(l_puc_V);

			l_puc_Param = (PUC)malloc(l_ul_SizeParam);

			memcpy(l_puc_Param,l_puc_V,l_ul_SizeParam);

			if (l_px_Mso100Struct->g_i_nb_RecordedILV != 0)
			{
				l_px_Mso100Struct->g_px_IlvTable[l_i_Cnt].m_x_Callback(	l_px_Mso100Struct->g_px_IlvTable[l_i_Cnt].m_pv_Context,
											l_uc_ILV_Type,
											l_puc_Param );
			}
			free(l_puc_Param);

			l_i_Ret = 0;
			break;
		}
	}

	return l_i_Ret;
}

/////////////////////////////////////////////////////////////////////
I MSO_GetLastEmbeddedError
(
	MORPHO_HANDLE		i_h_Mso100Handle
)
{
	PT_MSO100_STRUCT	l_px_Mso100Struct;

	if(i_h_Mso100Handle == NULL)
	{
	    return MSO_BAD_PARAMETER;
	}

	l_px_Mso100Struct = ((PT_MSO100_STRUCT)i_h_Mso100Handle);

	return l_px_Mso100Struct->g_i_EmbeddedError;
}

