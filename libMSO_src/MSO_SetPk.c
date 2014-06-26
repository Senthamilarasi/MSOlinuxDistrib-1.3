#include <stdio.h>

#include "libMSO.h"

#include "MSO100.h"
#include "SpIlv.h"

/***********************************************************************
Description :
 
***********************************************************************/
I MSO_SetPk 
(
	UC		i_uc_NbPk,
	PT_BUF_PK	i_px_Pk,
	PT_BUF		io_px_MainIlv,
	PUL		io_pul_MainIlvSize
)
{
	I		l_i_Ret;
	UC		l_auc_TempIlv[1024];
	T_BUF		l_x_PKIlv;
	UC		l_uc_Cnt;
	UL		l_ul_PkIlvSize;

	l_x_PKIlv.m_ul_Size = sizeof(l_auc_TempIlv);
	l_x_PKIlv.m_puc_Buf = l_auc_TempIlv;

	l_i_Ret = 0;

	if(i_px_Pk->m_uc_IdPk == ID_X984)
	{
		l_i_Ret = MSO_SetTkb (
			&(i_px_Pk[0]),			//PT_BUF_PK	i_px_X984,
			i_uc_NbPk,			//UC		i_uc_PkIndex,
			io_px_MainIlv,			//PT_BUF	io_px_MainIlv,
			io_pul_MainIlvSize		//PUL		io_pul_MainIlvSize
			);

		if(l_i_Ret)
			return l_i_Ret;
	}
	else
	{		
		for( l_uc_Cnt=0; l_uc_Cnt<i_uc_NbPk; l_uc_Cnt++)
		{
			if( (i_px_Pk[l_uc_Cnt].m_uc_IdPk == ID_PKMAT)		||
				(i_px_Pk[l_uc_Cnt].m_uc_IdPk == ID_PKMAT_NORM)	||
				(i_px_Pk[l_uc_Cnt].m_uc_IdPk == ID_PKCOMP)	||
				(i_px_Pk[l_uc_Cnt].m_uc_IdPk == ID_PKCOMP_NORM) ||
				(i_px_Pk[l_uc_Cnt].m_uc_IdPk == ID_PKBASE))
			{
				l_i_Ret = ILV_SetILV(
					   &l_x_PKIlv, 
					   (US)i_px_Pk[l_uc_Cnt].m_uc_IdPk, 
					   &(i_px_Pk[l_uc_Cnt].m_x_Buf),
					   &l_ul_PkIlvSize);

				if(l_i_Ret)
					return l_i_Ret;

				l_i_Ret = ILV_AddILV(
					   io_px_MainIlv,			//buffer
					   l_ul_PkIlvSize,			//taille des données à ajouter
					   l_x_PKIlv.m_puc_Buf,			//données à ajouter
					   io_pul_MainIlvSize);			//taille de l'ILV

				if(l_i_Ret)
					return l_i_Ret;
			}
			else if (
				(i_px_Pk[l_uc_Cnt].m_uc_IdPk == ID_ISO_PK_DATA_ANSI_378)	||
				(i_px_Pk[l_uc_Cnt].m_uc_IdPk == ID_ISO_PK_DATA_ISO_FMR)		||
				(i_px_Pk[l_uc_Cnt].m_uc_IdPk == ID_ISO_PK_DATA_ISO_FMC_NS)	||
				(i_px_Pk[l_uc_Cnt].m_uc_IdPk == ID_ISO_PK_DATA_ISO_FMC_CS)	||
				(i_px_Pk[l_uc_Cnt].m_uc_IdPk == ID_ISO_PK_DATA_MINEX_A)		)
			{
				/*
				 * ISO Pk
				 */

				l_i_Ret = MSO_SetIsoPk(&(i_px_Pk[l_uc_Cnt]), io_px_MainIlv, io_pul_MainIlvSize);
				if(l_i_Ret)
					return l_i_Ret;
			}
			else
				return ERROR_APPLI_BAD_PARAMETER;
		}
	}

	return l_i_Ret;
}


/***********************************************************************
Description :
 
***********************************************************************/
I MSO_GetPk 
(
	UC			i_uc_NbFinger,
	PUC			i_puc_ReceivedIlv,
	PUL			i_pul_ReceivedIlvSize,
	PT_EXPORT_PK		o_px_ExportPk
)


{
	UC	l_uc_Cnt;
	US	l_us_I;
	UL	l_ul_L;

	I	l_i_Ret;
	I	l_i_AlreadyGetX984;

	PUC	l_puc_TempILV;
	PUC 	l_puc_V;



	l_uc_Cnt = 0;
	l_i_AlreadyGetX984 = 0;
	while( l_uc_Cnt < i_uc_NbFinger)
	{
		if (l_i_AlreadyGetX984 == 0)
		{
			l_puc_TempILV = i_puc_ReceivedIlv + *i_pul_ReceivedIlvSize + SizeOfIandL(i_puc_ReceivedIlv);
		
			l_i_Ret = ILV_GetILV( l_puc_TempILV, &l_us_I, &l_ul_L, &l_puc_V);
			if (l_i_Ret)
				return l_i_Ret;

			if(l_ul_L > o_px_ExportPk->io_px_BiometricData[l_uc_Cnt].m_ul_Size)
				return ILV_ERROR_NOT_ENOUGH_MEMORY;

			*i_pul_ReceivedIlvSize = *i_pul_ReceivedIlvSize + SizeOfIandL(l_puc_TempILV);

			o_px_ExportPk->io_px_BiometricData[l_uc_Cnt].m_ul_Size = l_ul_L; 

			l_i_Ret = ILV_GetValue(i_puc_ReceivedIlv, i_pul_ReceivedIlvSize, o_px_ExportPk->io_px_BiometricData[l_uc_Cnt].m_puc_Buf, o_px_ExportPk->io_px_BiometricData[l_uc_Cnt].m_ul_Size);

			if (l_us_I == ID_X984)
				l_i_AlreadyGetX984 = 1;
		}
		else
		{
			/*
			 * When X984 templates have already been get
			 * Others buffers must be set to 0
			 */
			o_px_ExportPk->io_px_BiometricData[l_uc_Cnt].m_ul_Size = 0;

		}

		l_uc_Cnt++;
	}
	
	return RETURN_NO_ERROR; 
}

