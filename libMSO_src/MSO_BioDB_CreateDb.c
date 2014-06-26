#include <stdio.h>
#include <string.h>

#include "libMSO.h"

#include "MSO100.h"
#include "SpIlv.h"


I MSO_BioDB_CreateDb 
(
	MORPHO_HANDLE	i_h_Mso100Handle,
	UC 		i_uc_IndexDB,
	US		i_us_UserMax, 
	UC		i_uc_NbFinger,
	UC		i_uc_NormalizedPK_Type,
	UC		i_uc_NbAddField,
	PT_FIELD 	i_px_AddField,
	PUC		o_puc_ILV_Status
)
{
	US			l_us_RequestId;
	PUC			l_puc_ILV_ReceiveBuffer;
	T_ILV_DB		l_x_IlvDb;
	T_ILV_ADD_FIELD		l_x_IlvAddField;

	I			l_i_Ret;
	UL			l_ul_ILVSize;

	UC			l_auc_TempIlv[255];
	US			l_us_TempI;
	UL			l_ul_TempIlvSize;

	UC			i;

	T_BUF			l_x_ILV_SendBuffer;
	T_BUF			l_x_V;
	T_BUF			l_x_TempIlv;
	T_BUF			l_x_bufField;
	UC			l_auc_ILV_SendBuffer[ILV_SENDBUFFER_SIZE];

	*o_puc_ILV_Status	= RETURN_NO_ERROR;

	l_us_RequestId		= ILV_CREATE_DB;

	l_x_IlvDb.m_uc_FlashType	= 0;
	l_x_IlvDb.m_uc_PkMax		= i_uc_NbFinger;
	l_x_IlvDb.m_uc_IndexDB		= i_uc_IndexDB;
	l_x_IlvDb.m_us_UserMax		= i_us_UserMax;

	l_x_ILV_SendBuffer.m_ul_Size	= sizeof(l_auc_ILV_SendBuffer);
	l_x_ILV_SendBuffer.m_puc_Buf	= l_auc_ILV_SendBuffer;

	l_x_V.m_ul_Size = sizeof(l_x_IlvDb);
	l_x_V.m_puc_Buf	= (PUC)	&l_x_IlvDb;

	l_i_Ret = ILV_SetILV(
			   &l_x_ILV_SendBuffer, 
			   l_us_RequestId, 
			   &l_x_V,
			   &l_ul_ILVSize);

	if(l_i_Ret)
		return l_i_Ret;

	l_x_TempIlv.m_ul_Size = 255;
	l_x_TempIlv.m_puc_Buf = l_auc_TempIlv;

	for( i=0; i<i_uc_NbAddField; i++)
	{
		l_x_IlvAddField.m_us_FieldMaxSize	= i_px_AddField[i].m_us_FieldMaxSize;
		memset(l_x_IlvAddField.m_auc_FieldName, 0, MAX_FIELD_NAME_LEN);
		strncpy((PC)l_x_IlvAddField.m_auc_FieldName, (PC)i_px_AddField[i].m_auc_FieldName,MAX_FIELD_NAME_LEN);

		if(i_px_AddField[i].m_uc_Right == PUBLIC)
			l_us_TempI = ID_FIELD;
		else if ( i_px_AddField[i].m_uc_Right == PRIVATE )
			l_us_TempI = ID_PRIVATE_FIELD;
		else
			return ILVERR_BADPARAMETER;

		l_x_bufField.m_ul_Size = sizeof(l_x_IlvAddField);
		l_x_bufField.m_puc_Buf = (PUC)	&l_x_IlvAddField;

		l_i_Ret = ILV_SetILV(
			   &l_x_TempIlv, 
			   l_us_TempI, 
			   &l_x_bufField,
			   &l_ul_TempIlvSize);

		if(l_i_Ret)
			return l_i_Ret;

		l_i_Ret = ILV_AddILV(
			   &l_x_ILV_SendBuffer,			//buffer
			   l_ul_TempIlvSize,			//taille des données à ajouter
			   l_auc_TempIlv,			//données à ajouter
			   &l_ul_ILVSize);			//taille de l'ILV

		if(l_i_Ret)
			return l_i_Ret;
	}	

	if (i_uc_NormalizedPK_Type)
	{
		l_x_TempIlv.m_puc_Buf = l_auc_TempIlv;
		l_x_TempIlv.m_ul_Size = sizeof(l_auc_TempIlv);
		
		l_us_TempI = ID_BIO_ALGO_PARAM;
		
		l_x_V.m_ul_Size = sizeof(i_uc_NormalizedPK_Type);
		l_x_V.m_puc_Buf = (PUC)&i_uc_NormalizedPK_Type;
		
		l_i_Ret = ILV_SetILV(
			&l_x_TempIlv, 
			l_us_TempI, 
			&l_x_V,
			&l_ul_TempIlvSize);
		
		if(l_i_Ret)
			return l_i_Ret;
		
		// Add the temporary ILV to the main ILV
		l_i_Ret = ILV_AddILV(
			&l_x_ILV_SendBuffer,			//buffer
			l_ul_TempIlvSize,			//taille des données à ajouter
			l_auc_TempIlv,				//données à ajouter
			&l_ul_ILVSize);				//taille de l'ILV
		
		if(l_i_Ret)
			return l_i_Ret;
	}

	//ILV request
	l_i_Ret = MSO_SendReceive( i_h_Mso100Handle,
				   l_us_RequestId,		//US	i_us_I,		// Identifier
				   l_auc_ILV_SendBuffer,	//PUC	i_puc_Frame,	// data to send
				   l_ul_ILVSize,		//UL	i_ul_len,	// number of bytes to write
				   &l_puc_ILV_ReceiveBuffer,	//PUC	*o_puc_dataOut,	// response buffer
				   &l_ul_ILVSize,		//PUL	o_pul_len,	// response length
				   NULL,			//PUC *o_ppuc_Data,	// ILV data 
				   o_puc_ILV_Status,	//PUC	o_puc_ILV_Status,
				   NULL
				 );
	
	MSO_FrameRelease(i_h_Mso100Handle,&l_puc_ILV_ReceiveBuffer);

	return l_i_Ret;
}

