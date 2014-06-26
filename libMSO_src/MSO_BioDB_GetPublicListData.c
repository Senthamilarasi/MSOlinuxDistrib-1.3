#include <stdio.h>
#include <string.h>

#include "libMSO.h"

#include "MSO100.h"
#include "SpIlv.h"

/***********************************************************************
descriptions :

	o_pul_EmbeddedError can be null.
***********************************************************************/
I MSO_BioDB_GetPublicListData 
(
	MORPHO_HANDLE			i_h_Mso100Handle,
	UC 				i_uc_IndexDB,
	UL				i_ul_UidData,
	PUL				io_pul_NbTranport,
	PT_TRANSPORT_PUBLIC_LIST_FIELD	io_ax_TransportPublicField,
	PUC				o_puc_ILV_Status
)
{

	US					l_us_RequestId;
	PUC					l_puc_ILV_ReceiveBuffer;
	I					l_i_Ret;
	UL					l_ul_ILVSize;
	T_BUF				l_x_ILV_SendBuffer;
	T_BUF				l_x_V;
	UC*					l_puc_V;
	US					l_us_IdFieldContent;
	UL					i;
	UL					l_ul_PosIlv;
	UL					l_ul_Pos;
	UL					l_ul_Datalength;
	UL					l_ul_UserNumber;
	UC					l_auc_ILV_SendBuffer[ILV_SENDBUFFER_SIZE];

	*o_puc_ILV_Status	 = RETURN_NO_ERROR;

	l_us_RequestId		= ILV_LIST_PUBLIC_FIELDS;

	l_x_ILV_SendBuffer.m_ul_Size	= sizeof(l_auc_ILV_SendBuffer);
	l_x_ILV_SendBuffer.m_puc_Buf	= l_auc_ILV_SendBuffer;

	l_x_V.m_ul_Size = sizeof(UC);
	l_x_V.m_puc_Buf	= &i_uc_IndexDB;

	l_i_Ret = ILV_SetILV(
			   &l_x_ILV_SendBuffer,			//buffer contenant l'ILV
			   l_us_RequestId,			//I de l'ILV 
			   &l_x_V,				//Buffer contenant le V
			   &l_ul_ILVSize);			//taille de l'ILV à envoyer

	if(l_i_Ret)
		return l_i_Ret;

	l_i_Ret = ILV_AddILV(
			   &l_x_ILV_SendBuffer,			//buffer contenant l'ILV
			   sizeof(i_ul_UidData),		//taille des données à ajouter
			   (PUC)&i_ul_UidData,			//données à ajouter
			   &l_ul_ILVSize);			//taille de l'ILV

	if(l_i_Ret)
		return l_i_Ret;

	//ILV request
	l_i_Ret = MSO_SendReceive(
				i_h_Mso100Handle,
				l_us_RequestId,			//US	i_us_I,			// Identifier
				l_auc_ILV_SendBuffer,		//PUC	i_puc_Frame,	// data to send
				l_ul_ILVSize,			//UL	i_ul_len,		// number of bytes to write
				&l_puc_ILV_ReceiveBuffer,	//PUC	*o_puc_dataOut,	// response buffer
				&l_ul_ILVSize,			//PUL	o_pul_len,		// response length
				&l_puc_V,			//PUC *o_ppuc_Data,		// ILV data 
				o_puc_ILV_Status,		//PUC	o_puc_ILV_Status,
				NULL);				//PUL	o_pul_InternalErrorCode)
	
	if (l_i_Ret || *o_puc_ILV_Status)
	{
		MSO_FrameRelease(i_h_Mso100Handle,&l_puc_ILV_ReceiveBuffer);
		return l_i_Ret;
	}

	memcpy(&l_ul_UserNumber,l_puc_V,sizeof(UL));
	l_ul_PosIlv = sizeof(UL);

	if(l_ul_UserNumber>*io_pul_NbTranport)
	{
		MSO_FrameRelease(i_h_Mso100Handle,&l_puc_ILV_ReceiveBuffer);
		return ILVERR_BADPARAMETER;
	}
	
	for(i = 0;i<l_ul_UserNumber;i++)
	{
		l_us_IdFieldContent = ILV_GetI(l_puc_V+l_ul_PosIlv);

		if(l_us_IdFieldContent != ID_FIELD_CONTENT)
		{
			MSO_FrameRelease(i_h_Mso100Handle,&l_puc_ILV_ReceiveBuffer);
			return ILVERR_BADPARAMETER;
		}

		l_ul_Pos = 0;

		l_i_Ret = ILV_GetValue(
						l_puc_V+l_ul_PosIlv,
						&l_ul_Pos,
						(PUC)&(io_ax_TransportPublicField[i].m_ul_UserIndex),
						sizeof(UL));

		if (l_i_Ret)
		{
			MSO_FrameRelease(i_h_Mso100Handle,&l_puc_ILV_ReceiveBuffer);
			return l_i_Ret;
		}

		l_i_Ret = ILV_GetValue(
						l_puc_V+l_ul_PosIlv,
						&l_ul_Pos,
						(PUC)&l_ul_Datalength,
						sizeof(UL));

		if (l_i_Ret)
		{
			MSO_FrameRelease(i_h_Mso100Handle,&l_puc_ILV_ReceiveBuffer);
			return l_i_Ret;
		}

		if(l_ul_Datalength > io_ax_TransportPublicField[i].m_ul_DataLenght)
		{
			MSO_FrameRelease(i_h_Mso100Handle,&l_puc_ILV_ReceiveBuffer);
			return ILVERR_BADPARAMETER;
		}

		io_ax_TransportPublicField[i].m_ul_DataLenght = l_ul_Datalength;

		l_i_Ret = ILV_GetValue(
						l_puc_V+l_ul_PosIlv,
						&l_ul_Pos,
						io_ax_TransportPublicField[i].m_puc_Data,
						l_ul_Datalength);

		if (l_i_Ret)
		{
			MSO_FrameRelease(i_h_Mso100Handle,&l_puc_ILV_ReceiveBuffer);
			return l_i_Ret;
		}

		l_ul_PosIlv += ILV_GetSize(l_puc_V+l_ul_PosIlv);
	}

	*io_pul_NbTranport = l_ul_UserNumber;

	MSO_FrameRelease(i_h_Mso100Handle,&l_puc_ILV_ReceiveBuffer);

	return l_i_Ret;
}

