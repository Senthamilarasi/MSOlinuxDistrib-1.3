#include <stdio.h>
#include <stdlib.h>

#include "libMSO.h"
#include "SpIlv.h"


/***********************************************************************
descriptions :
 
	ID_X984_BIO_TOKEN
		ID_X984_PARAM
		ID_X984

***********************************************************************/
I MSO_SetTkb 
(
	PT_BUF_PK	i_px_X984,
	UC		i_uc_PkIndex,
	PT_BUF		io_px_MainIlv,
	PUL		io_pul_MainIlvSize
)
{
	I		l_i_Ret;
	US		l_us_TempI;
	T_BUF		l_x_X984Ilv;
	T_BUF		l_x_ParamIlv;
	T_BUF		l_x_Param;
	T_BUF		l_x_TokenIlv;
	UL		l_ul_X984IlvSize;
	UL		l_ul_ParamIlvSize;
	UL		l_ul_TokenIlvSize;

	l_x_ParamIlv.m_puc_Buf = NULL;
	l_x_X984Ilv.m_puc_Buf = NULL;
	l_x_TokenIlv.m_puc_Buf = NULL;

	l_x_ParamIlv.m_ul_Size = sizeof(i_uc_PkIndex) + I_L_MAX_SIZE;
	l_x_ParamIlv.m_puc_Buf = malloc(l_x_ParamIlv.m_ul_Size);

	l_x_Param.m_ul_Size = sizeof(i_uc_PkIndex);
	l_x_Param.m_puc_Buf = &i_uc_PkIndex;

	l_us_TempI = (US)ID_X984_PARAM;

	l_i_Ret = ILV_SetILV(
		   &l_x_ParamIlv, 
		   l_us_TempI, 
		   &l_x_Param,
		   &l_ul_ParamIlvSize);

	if(!l_i_Ret)
	{
		l_x_ParamIlv.m_ul_Size = l_ul_ParamIlvSize;
		
		l_x_X984Ilv.m_ul_Size = i_px_X984->m_x_Buf.m_ul_Size + I_L_MAX_SIZE;
		l_x_X984Ilv.m_puc_Buf = malloc(l_x_X984Ilv.m_ul_Size);

		l_us_TempI = (US)ID_X984;

		l_i_Ret = ILV_SetILV(
			   &l_x_X984Ilv, 
			   l_us_TempI, 
			   &(i_px_X984->m_x_Buf),
			   &l_ul_X984IlvSize);
	}

	if(!l_i_Ret)
	{
		l_x_X984Ilv.m_ul_Size = l_ul_X984IlvSize;

		l_x_TokenIlv.m_ul_Size = l_ul_X984IlvSize + l_ul_ParamIlvSize + I_L_MAX_SIZE;
		l_x_TokenIlv.m_puc_Buf = malloc(l_x_TokenIlv.m_ul_Size);

		l_us_TempI = (US)ID_TKB;

		l_i_Ret = ILV_SetILV(
			   &l_x_TokenIlv, 
			   l_us_TempI, 
			   &l_x_ParamIlv,
			   &l_ul_TokenIlvSize);
	}

	if(!l_i_Ret)
	{	
		l_i_Ret = ILV_AddILV(
			   &l_x_TokenIlv,				//buffer
			   l_ul_X984IlvSize,			//taille des données à ajouter
			   l_x_X984Ilv.m_puc_Buf,		//données à ajouter
			   &l_ul_TokenIlvSize);			//taille de l'ILV
	}

	if(!l_i_Ret)
	{
		l_i_Ret = ILV_AddILV(
			   io_px_MainIlv,				//buffer
			   l_ul_TokenIlvSize,			//taille des données à ajouter
			   l_x_TokenIlv.m_puc_Buf,		//données à ajouter
			   io_pul_MainIlvSize);			//taille de l'ILV
	}

	if(l_x_ParamIlv.m_puc_Buf)
		free(l_x_ParamIlv.m_puc_Buf);

	if(l_x_X984Ilv.m_puc_Buf)
		free(l_x_X984Ilv.m_puc_Buf);

	if(l_x_TokenIlv.m_puc_Buf)
		free(l_x_TokenIlv.m_puc_Buf);

	return l_i_Ret;
}

