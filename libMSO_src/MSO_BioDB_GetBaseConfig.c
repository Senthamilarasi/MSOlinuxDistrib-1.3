#include <stdio.h>
#include <string.h>

#include "libMSO.h"

#include "MSO100.h"
#include "SpIlv.h"


/***********************************************************************
descriptions :
	o_pul_EmbeddedError can be null.
***********************************************************************/
I MSO_BioDB_GetBaseConfig 
(
	MORPHO_HANDLE	i_h_Mso100Handle,
	UC 		i_uc_IndexDB,
	PUC		o_puc_FingerNb,
	PUL		o_pul_MaxRecord,
	PUL		o_pul_CurrentRecord,
	PUL		io_pul_AddFieldNb,
	PT_FIELD 	o_px_AddField,
	PUC		o_puc_NormalizedPK_Type,	//can be NULL
	PUC		o_puc_ILV_Status,
	PUL		o_pul_EmbeddedError
)
{
	US		l_us_RequestId;
	PUC		l_puc_ILV_ReceiveBuffer;
	I		l_i_Ret;
	UL		l_ul_ILVSize;
	T_BUF		l_x_ILV_SendBuffer;
	T_BUF		l_x_V;
	UL		l_ul_ReceiveL;
	UL		l_ul_SizeIlvTimeStamp;
	I		l_i_NbAddFieldData;
	PC		l_pc_i;
	PT_ILV_BASE_CONFIG	l_px_IlvBaseConfig;
	US		l_us_TempI;
	UL		l_ul_TempL;
	PUC		l_puc_TempV;
	PT_ILV_ADD_FIELD	l_px_AddField;
	PUC		l_puc_V;
	UC		l_auc_ILV_SendBuffer[ILV_SENDBUFFER_SIZE];

	*o_puc_ILV_Status	 = RETURN_NO_ERROR;

	if(o_puc_NormalizedPK_Type)
		*o_puc_NormalizedPK_Type = 0;

	if(o_pul_EmbeddedError)
		*o_pul_EmbeddedError = RETURN_NO_ERROR;

// Set ILV
	l_us_RequestId	= ILV_GET_BASE_CONGIG;

	l_x_ILV_SendBuffer.m_ul_Size = sizeof(l_auc_ILV_SendBuffer);
	l_x_ILV_SendBuffer.m_puc_Buf = l_auc_ILV_SendBuffer;

	l_x_V.m_ul_Size = sizeof(UC);
	l_x_V.m_puc_Buf	= &i_uc_IndexDB;

	l_i_Ret = ILV_SetILV(
			   &l_x_ILV_SendBuffer, 
			   l_us_RequestId, 
			   &l_x_V,
			   &l_ul_ILVSize);

	if(l_i_Ret)
		return l_i_Ret;

// Send and Receive ILV

	l_i_Ret = MSO_SendReceive(
				i_h_Mso100Handle,
				l_us_RequestId,			//US	i_us_I,		// Identifier
				l_auc_ILV_SendBuffer,		//PUC	i_puc_Frame,	// data to send
				l_ul_ILVSize,			//UL	i_ul_len,	// number of bytes to write
				&l_puc_ILV_ReceiveBuffer,	//PUC  *o_puc_dataOut,	// response buffer
				&l_ul_ILVSize,			//PUL	o_pul_len,	// response length
				&l_puc_V,			//PUC  *o_ppuc_Data,	// ILV data 
				o_puc_ILV_Status,		//PUC	o_puc_ILV_Status,
				(PI)o_pul_EmbeddedError);	//PUL	o_pul_InternalErrorCode)
	
	if (l_i_Ret || *o_puc_ILV_Status)
	{
		return l_i_Ret;
	}

	l_puc_V--;

	l_ul_ReceiveL = ILV_GetL(l_puc_ILV_ReceiveBuffer);

	if(l_ul_ReceiveL >=  sizeof(T_ILV_BASE_CONFIG))
	{
		l_px_IlvBaseConfig = (PT_ILV_BASE_CONFIG)l_puc_V;

		*o_puc_FingerNb		= l_px_IlvBaseConfig->m_uc_Finger;
		*o_pul_MaxRecord	= l_px_IlvBaseConfig->m_ul_MaxRecord;
		*o_pul_CurrentRecord= l_px_IlvBaseConfig->m_ul_CurrentRecord;

		if (o_px_AddField!=NULL)
		{
			if(*io_pul_AddFieldNb >= l_px_IlvBaseConfig->m_ul_AddField)
				*io_pul_AddFieldNb	= l_px_IlvBaseConfig->m_ul_AddField;
			else
				l_i_Ret = ILVERR_ERROR;			
		}

		if(l_px_IlvBaseConfig->m_ul_FreeRecord != l_px_IlvBaseConfig->m_ul_MaxRecord - l_px_IlvBaseConfig->m_ul_CurrentRecord)
			l_i_Ret = ILVERR_ERROR;
	}
	else
		l_i_Ret = ILVERR_BADPARAMETER;

	if(!l_i_Ret)
	{

		l_ul_SizeIlvTimeStamp = ILV_GetSize(l_puc_V+sizeof(T_ILV_BASE_CONFIG));

		l_i_NbAddFieldData = 0;

		for(	l_pc_i = ((PC)(l_puc_V) + (sizeof(T_ILV_BASE_CONFIG) + l_ul_SizeIlvTimeStamp));
				l_pc_i < ((PC)(l_puc_V) + l_ul_ReceiveL); )
		{
			l_i_Ret = ILV_GetILV((PUC)l_pc_i, &l_us_TempI, &l_ul_TempL, &l_puc_TempV);

			if(l_i_Ret)
				return l_i_Ret;
			
			if(l_ul_TempL > 2+MAX_FIELD_NAME_LEN)
				return ILVERR_ERROR;

			if(l_us_TempI == ID_BIO_ALGO_PARAM)
			{
				if(o_puc_NormalizedPK_Type)
					*o_puc_NormalizedPK_Type = *l_puc_TempV;

				l_pc_i += ILV_GetSize((PUC)l_pc_i);
			}
			else
			{
				if((l_us_TempI != ID_FIELD) && (l_us_TempI != ID_PRIVATE_FIELD))
					return ILVERR_ERROR;

				l_px_AddField = (PT_ILV_ADD_FIELD)l_puc_TempV;

				if (o_px_AddField!=NULL)
				{
					o_px_AddField[l_i_NbAddFieldData].m_auc_FieldName[MAX_FIELD_NAME_LEN]='\0';

					o_px_AddField[l_i_NbAddFieldData].m_us_FieldMaxSize		= l_px_AddField->m_us_FieldMaxSize;	
					strncpy((PC)o_px_AddField[l_i_NbAddFieldData].m_auc_FieldName,(PC)l_px_AddField->m_auc_FieldName,MAX_FIELD_NAME_LEN);

					if(l_us_TempI == ID_FIELD)
						o_px_AddField[l_i_NbAddFieldData].m_uc_Right = PUBLIC;
					else if ( l_us_TempI == ID_PRIVATE_FIELD )
						o_px_AddField[l_i_NbAddFieldData].m_uc_Right = PRIVATE;
					else
						return ILVERR_BADPARAMETER;
				}

				l_pc_i += ILV_GetSize((PUC)l_pc_i);

				l_i_NbAddFieldData++;
			}
		}
	}

	MSO_FrameRelease(i_h_Mso100Handle,&l_puc_ILV_ReceiveBuffer);

	return l_i_Ret;
}

