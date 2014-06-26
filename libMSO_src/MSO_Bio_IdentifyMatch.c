#include <stdio.h>

#include "libMSO.h"

#include "MSO100.h"
#include "SpIlv.h"


/***********************************************************************
descriptions :

***********************************************************************/
I MSO_Bio_IdentifyMatch 
(
	MORPHO_HANDLE	i_h_Mso100Handle,
	UC 		i_uc_UidDB,
	US		i_us_MatchingTreshold,
	UC		i_uc_NbPk,
	PT_BUF_PK	i_px_Pk,
	PUC		o_puc_MatchingResult,
	PUL		o_pul_UserDBIndex,
	PT_BUF		o_px_UserID,
	PUL		o_pul_score,
	PUC		o_puc_ILV_Status
)
{
	/* Variables declarations */
	I		l_i_Ret;
	UL		l_ul_ILVSize;
	US		l_us_RequestId;
	UC		l_uc_I;
	PUC		l_puc_ILV_ReceiveBuffer;
	UL		l_ul_Pos;
	T_BUF		l_x_MainIlv;
	UC		l_uc_export_matching_score;
	US		l_us_L;
	UL		l_ul_ReceiveILVSize;
	UC		l_auc_ILV_SendBuffer[ILV_SENDBUFFER_SIZE];
	
	/////////////////////////////////////////
	// Initialisations
	/////////////////////////////////////////

	*o_pul_UserDBIndex=0xFFFFFFFF;
	o_px_UserID->m_ul_Size = 0;
	
	*o_puc_ILV_Status = RETURN_NO_ERROR;
	
	l_us_RequestId = ILV_IDENTIFY_MATCH;

//build request ILV
	l_i_Ret = ILV_Init(l_auc_ILV_SendBuffer, &l_ul_ILVSize, l_us_RequestId);
	if (l_i_Ret<0)
	{
		return l_i_Ret;
	}

//add db id
	l_i_Ret = ILV_AddValue(l_auc_ILV_SendBuffer, &l_ul_ILVSize, (PUC)&i_uc_UidDB,sizeof(i_uc_UidDB));
	if (l_i_Ret<0)
	{
		return l_i_Ret;
	}

//add matching threshold
	l_i_Ret = ILV_AddValue(l_auc_ILV_SendBuffer, &l_ul_ILVSize, (PUC)&i_us_MatchingTreshold,sizeof(i_us_MatchingTreshold));
	if (l_i_Ret<0)
	{
		return l_i_Ret;
	}

	l_x_MainIlv.m_puc_Buf = l_auc_ILV_SendBuffer;
	l_x_MainIlv.m_ul_Size = sizeof(l_auc_ILV_SendBuffer);

	l_i_Ret = MSO_SetPk (
			i_uc_NbPk,			//UC		i_uc_NbPk,
			i_px_Pk,			//PT_BUF_PK	i_px_Pk,
			&l_x_MainIlv,			//PT_BUF	io_px_MainIlv,
			&l_ul_ILVSize			//PUL		io_pul_MainIlvSize
			);

	if(l_i_Ret)
		return l_i_Ret;

//add export matching score optional parameter
	if (o_pul_score!= NULL)
	{
		*o_pul_score=0;
		l_uc_I = ID_MATCHING_SCORE;
		l_i_Ret = ILV_AddValue(l_auc_ILV_SendBuffer, &l_ul_ILVSize, (PUC)&l_uc_I,sizeof(l_uc_I));
		if (l_i_Ret<0)
		{
			return l_i_Ret;
		}

		//L sur 2 octets !
		l_us_L = sizeof(l_uc_export_matching_score);
		l_i_Ret = ILV_AddValue(l_auc_ILV_SendBuffer, &l_ul_ILVSize, (PUC)&l_us_L,sizeof(US));
		if (l_i_Ret<0)
		{
			return l_i_Ret;
		}

		l_uc_export_matching_score = TRUE;
		l_i_Ret = ILV_AddValue(l_auc_ILV_SendBuffer, &l_ul_ILVSize, (PUC)&l_uc_export_matching_score,l_us_L);
		if (l_i_Ret<0)
		{
			return l_i_Ret;
		}
	}
	
	/////////////////////////////////////////
	// Send ILV request
	/////////////////////////////////////////
	
		l_i_Ret = MSO_SendReceiveTimeout( i_h_Mso100Handle,
						  WRITE_TIMEOUT, 
						  WRITE_TIMEOUT + IDENT_TIMEOUT,
						  l_us_RequestId,
						  l_auc_ILV_SendBuffer, 
						  l_ul_ILVSize, 
						  &l_puc_ILV_ReceiveBuffer, 
						  &l_ul_ReceiveILVSize,
						  NULL,			//PUC *o_ppuc_Data,	// ILV data 
						  o_puc_ILV_Status,	//PUC	o_puc_ILV_Status,
					          NULL
						);
		
	if (l_i_Ret || *o_puc_ILV_Status)
	{
		return l_i_Ret;
	}

	l_ul_Pos = 1;	//o_puc_ILV_Status
		
	//matching result
	l_i_Ret = ILV_GetValue(l_puc_ILV_ReceiveBuffer, &l_ul_Pos, o_puc_MatchingResult, sizeof(UC));
	if (l_i_Ret)
	{
		return l_i_Ret;
	}

	if (*o_puc_MatchingResult==ILVSTS_HIT)
	{
		//user index
		l_i_Ret = ILV_GetValue(l_puc_ILV_ReceiveBuffer, &l_ul_Pos, (PUC)o_pul_UserDBIndex, sizeof(*o_pul_UserDBIndex));
		if (l_i_Ret)
		{
			return l_i_Ret;
		}

		//user id ID
		l_i_Ret = ILV_GetValue(l_puc_ILV_ReceiveBuffer, &l_ul_Pos, (PUC)&l_uc_I, sizeof(l_uc_I));
		if (l_i_Ret)
		{
			return l_i_Ret;
		}

		//user id L
		o_px_UserID->m_ul_Size = 0;
		l_i_Ret = ILV_GetValue(l_puc_ILV_ReceiveBuffer, &l_ul_Pos, (PUC)&o_px_UserID->m_ul_Size, sizeof(US));
		if (l_i_Ret)
		{
			return l_i_Ret;
		}

		//user id V
		l_i_Ret = ILV_GetValue(l_puc_ILV_ReceiveBuffer, &l_ul_Pos, (PUC)o_px_UserID->m_puc_Buf, o_px_UserID->m_ul_Size);
		if (l_i_Ret)
		{
			return l_i_Ret;
		}
	}

	if ((o_pul_score!= NULL)&&(l_ul_Pos<ILV_GetL(l_puc_ILV_ReceiveBuffer)))
	{
		l_i_Ret = ILV_GetValue(l_puc_ILV_ReceiveBuffer, &l_ul_Pos, (PUC) &l_uc_I, sizeof(l_uc_I));
		if (l_i_Ret)
		{
			return l_i_Ret;
		}
		
		if (l_uc_I==ID_MATCHING_SCORE)
		{
			l_i_Ret = ILV_GetValue(l_puc_ILV_ReceiveBuffer, &l_ul_Pos, (PUC) &l_ul_ILVSize, sizeof(US));
			if (l_i_Ret)
			{
				return l_i_Ret;
			}
			l_i_Ret = ILV_GetValue(l_puc_ILV_ReceiveBuffer, &l_ul_Pos, (PUC)o_pul_score, sizeof(*o_pul_score));
			if (l_i_Ret)
			{
				return l_i_Ret;
			}
		}
	}
	
	MSO_FrameRelease(i_h_Mso100Handle,&l_puc_ILV_ReceiveBuffer);

	return l_i_Ret;
}

