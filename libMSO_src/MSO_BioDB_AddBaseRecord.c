#include <stdio.h>
#include <string.h>

#include "libMSO.h"

#include "MSO100.h"
#include "SpIlv.h"

I MSO_BioDBAddBaseRecord
(
	MORPHO_HANDLE		i_h_Mso100Handle,
	UC 			i_uc_IndexDB,
	UC			i_uc_NbPk,
	PT_BUF_PK		i_px_Pk,
	PT_BUF			i_px_UserId,
	UC			i_uc_NbAddField,
	PT_BUF			i_px_AddField,
	PUL			o_pul_IndexUser,
	PUC			o_puc_ILV_Status,
	PUC			o_puc_Base_Status,
	BOOL			i_b_NoCheckOnTemplate
)
{
	US			l_us_RequestId;
	PUC			l_puc_ILV_ReceiveBuffer;
	I			l_i_Ret;
	UL			l_ul_ILVSize;
	UC			l_auc_TempIlv[1024];
	US			l_us_TempI;
	UL			l_ul_TempIlvSize;
	UC			i;
	T_BUF			l_x_ILV_SendBuffer;
	T_BUF			l_x_V;
	T_BUF			l_x_TempIlv;
	PUC			l_puc_V;
	UL			l_ul_ReceiveILVSize;
	UC			l_auc_ILV_SendBuffer[ILV_SENDBUFFER_SIZE];

	*o_puc_ILV_Status	= RETURN_NO_ERROR;
	*o_pul_IndexUser 	= 0xFFFFFFFF;

	l_us_RequestId		= ILV_ADD_RECORD;

	l_x_ILV_SendBuffer.m_ul_Size	= sizeof(l_auc_ILV_SendBuffer);
	l_x_ILV_SendBuffer.m_puc_Buf	= l_auc_ILV_SendBuffer;

	l_x_V.m_ul_Size = sizeof(i_uc_IndexDB);
	l_x_V.m_puc_Buf	= &i_uc_IndexDB;

	l_i_Ret = ILV_SetILV(
			   &l_x_ILV_SendBuffer, 
			   l_us_RequestId, 
			   &l_x_V,
			   &l_ul_ILVSize);

	if(l_i_Ret)
		return l_i_Ret;

	l_i_Ret = MSO_SetPk (
			i_uc_NbPk,		//UC		i_uc_NbPk,
			i_px_Pk,		//PT_BUF_PK	i_px_Pk,
			&l_x_ILV_SendBuffer,	//PT_BUF	io_px_MainIlv,
			&l_ul_ILVSize		//PUL		io_pul_MainIlvSize
			);

	if(l_i_Ret)
		return l_i_Ret;

	l_x_TempIlv.m_ul_Size = 1024;
	l_x_TempIlv.m_puc_Buf = l_auc_TempIlv;	

	l_us_TempI = ID_USER_ID;

	l_i_Ret = ILV_SetILV(
		   &l_x_TempIlv, 
		   l_us_TempI, 
		   i_px_UserId,
		   &l_ul_TempIlvSize);

	if(l_i_Ret)
		return l_i_Ret;

	l_i_Ret = ILV_AddILV(
		   &l_x_ILV_SendBuffer,	
		   l_ul_TempIlvSize,
		   l_auc_TempIlv,
		   &l_ul_ILVSize);

	if(l_i_Ret)
		return l_i_Ret;

	for( i=0; i<i_uc_NbAddField; i++)
	{
		l_us_TempI = ID_PUC_DATA;

		l_i_Ret = ILV_SetILV(
			   &l_x_TempIlv, 
			   l_us_TempI, 
			   &(i_px_AddField[i]),
			   &l_ul_TempIlvSize);

		if(l_i_Ret)
			return l_i_Ret;

		l_i_Ret = ILV_AddILV(
			   &l_x_ILV_SendBuffer,	
			   l_ul_TempIlvSize,
			   l_auc_TempIlv,
			   &l_ul_ILVSize);

		if(l_i_Ret)
			return l_i_Ret;
	}	

	if ( i_b_NoCheckOnTemplate == TRUE )
	{
		UC	l_uc_NoCheck = 0x01;

		l_us_TempI = ID_NO_CHECK_ON_TEMPLATE;

		l_x_V.m_ul_Size = sizeof(l_uc_NoCheck);
		l_x_V.m_puc_Buf	= &l_uc_NoCheck;

		l_i_Ret = ILV_SetILV(
			   &l_x_TempIlv, 
			   l_us_TempI, 
			   &l_x_V,
			   &l_ul_TempIlvSize);

		if(l_i_Ret)
			return l_i_Ret;

		l_i_Ret = ILV_AddILV(
			   &l_x_ILV_SendBuffer,	
			   l_ul_TempIlvSize,
			   l_auc_TempIlv,
			   &l_ul_ILVSize);

		if(l_i_Ret)
			return l_i_Ret;
	}

//ILV request

		l_i_Ret = MSO_SendReceiveTimeout(	i_h_Mso100Handle,
							WRITE_TIMEOUT, 
							WRITE_TIMEOUT + IDENT_TIMEOUT,
							l_us_RequestId,
							l_auc_ILV_SendBuffer, 
							l_ul_ILVSize, 
							&l_puc_ILV_ReceiveBuffer, 
							&l_ul_ReceiveILVSize,
							&l_puc_V,
							o_puc_ILV_Status,
							NULL);

	if (l_i_Ret || *o_puc_ILV_Status)
	{
		MSO_FrameRelease(i_h_Mso100Handle,&l_puc_ILV_ReceiveBuffer);
		return l_i_Ret;
	}
	
	//get base status
	*o_puc_Base_Status = *(l_puc_V);
	
	//get user index
	if (*o_puc_Base_Status == ILVSTS_OK)
		memcpy(o_pul_IndexUser,l_puc_V+sizeof(UC),sizeof(UL));

	MSO_FrameRelease(i_h_Mso100Handle,&l_puc_ILV_ReceiveBuffer);

	return l_i_Ret;
}

