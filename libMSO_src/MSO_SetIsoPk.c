#include <stdio.h>

#include "libMSO.h"
#include "MSO100.h"
#include "SpIlv.h"

I MSO_SetIsoPk
(			
	PT_BUF_PK	i_px_Pk,
	PT_BUF		io_px_MainIlv,
	PUL		io_pul_MainIlvSize
)
{

	I		l_i_Ret;
	UC		l_uc_TempIlv[1024];
	UC		l_uc_TempIlv2[1024];
	UC		l_uc_Temp;

	UL		l_ul_SizeIlvTemp;
	UL		l_ul_SizeIlvTemp2;

	T_BUF		l_x_IlvBuffer;
	T_BUF		l_x_IlvBuffer2;

	l_x_IlvBuffer.m_puc_Buf = l_uc_TempIlv;
	l_x_IlvBuffer2.m_puc_Buf = l_uc_TempIlv2;


	l_x_IlvBuffer.m_ul_Size = 1024;
	l_x_IlvBuffer2.m_ul_Size = 1024;

	/*
	 * ILV ID_ISO_PK
	 */

	l_i_Ret = ILV_Init( l_x_IlvBuffer.m_puc_Buf, &l_ul_SizeIlvTemp, ID_ISO_PK);
	if(l_i_Ret)
		return l_i_Ret;

	
	/*
	 * ILV ID_ISO_PK_PARAM
	 */

	l_i_Ret = ILV_Init( l_uc_TempIlv2, &l_ul_SizeIlvTemp2, ID_ISO_PK_PARAM);
	if(l_i_Ret)
		return l_i_Ret;

	l_i_Ret = ILV_AddValue(l_uc_TempIlv2, &l_ul_SizeIlvTemp2, &(i_px_Pk->m_uc_Index), 1);
	if(l_i_Ret)
		return l_i_Ret;

	if (i_px_Pk->m_uc_Index == 0xFF)
		l_uc_Temp = 1;
	else
		l_uc_Temp = 0;

	l_i_Ret = ILV_AddValue(l_uc_TempIlv2, &l_ul_SizeIlvTemp2, &l_uc_Temp, 1);

	l_i_Ret = ILV_AddILV(&l_x_IlvBuffer, l_ul_SizeIlvTemp2, l_x_IlvBuffer2.m_puc_Buf, &l_ul_SizeIlvTemp);
	if(l_i_Ret)
		return l_i_Ret;

	/*
	 * ILV ID_ISO_PK_DATA
	 */
	l_i_Ret = ILV_Init( l_uc_TempIlv2, &l_ul_SizeIlvTemp2, i_px_Pk->m_uc_IdPk);
	if(l_i_Ret)
		return l_i_Ret;

	l_i_Ret = ILV_AddValue(l_uc_TempIlv2, &l_ul_SizeIlvTemp2, i_px_Pk->m_x_Buf.m_puc_Buf, i_px_Pk->m_x_Buf.m_ul_Size);
	if(l_i_Ret)
		return l_i_Ret;

	l_i_Ret = ILV_AddILV(&l_x_IlvBuffer, l_ul_SizeIlvTemp2, l_x_IlvBuffer2.m_puc_Buf, &l_ul_SizeIlvTemp);
	if(l_i_Ret)
		return l_i_Ret;


	/*
	 * Write data in main ILV
	 */

	l_i_Ret = ILV_AddILV(io_px_MainIlv, l_ul_SizeIlvTemp, l_x_IlvBuffer.m_puc_Buf, io_pul_MainIlvSize);
	if(l_i_Ret)
		return l_i_Ret;

	return RETURN_NO_ERROR;
}

