#include <stdio.h>

#include "libMSO.h"

#include "MSO100.h"
#include "SpIlv.h"

#define MAX_DATA_REF		10
#define MAX_BIO_DATA_SIZE	512

/***********************************************************************
descriptions :

***********************************************************************/
I MSO_Bio_VerifyMatch 
(
	MORPHO_HANDLE	i_h_Mso100Handle,
	UC		i_uc_NbPkSrc,
	PT_BUF_PK	i_px_PkSrc,
	UC		i_uc_NbPkRef,
	PT_BUF_PK	i_px_PkRef,
	US		i_us_MatchingTreshold,
	PUC		o_puc_MatchingResult,
	PUC		o_puc_ListRefIndex,
	PUL		o_pul_score,
	PUC		o_puc_ILV_Status
)
{
	/* Variables declarations */
	I		l_i_Ret;
	UL		l_ul_ILVSize;
	US		l_us_RequestId;
	UC*		l_puc_V;
	PUC		l_puc_ILV_ReceiveBuffer;
	UC		l_auc_ILV_SendBuffer[ILV_SENDBUFFER_SIZE+MAX_DATA_REF*MAX_BIO_DATA_SIZE + MAX_BIO_DATA_SIZE];
	UL		l_ul_Pos;
	T_BUF	l_x_MainIlv;
	UC		l_uc_export_matching_score;
	UC		l_uc_I;
	US		l_us_L;
	
	/////////////////////////////////////////
	// Initialisations
	/////////////////////////////////////////
	
	*o_puc_ILV_Status = RETURN_NO_ERROR;
	
	l_us_RequestId = ILV_VERIFY_MATCH;

//build request ILV
	l_i_Ret = ILV_Init(l_auc_ILV_SendBuffer, &l_ul_ILVSize, l_us_RequestId);
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
//add pk src
	l_x_MainIlv.m_puc_Buf = l_auc_ILV_SendBuffer;
	l_x_MainIlv.m_ul_Size = sizeof(l_auc_ILV_SendBuffer);

	l_i_Ret = MSO_SetPk (
			i_uc_NbPkSrc,			//UC		i_uc_NbPk,
			i_px_PkSrc,			//PT_BUF_PK	i_px_Pk,
			&l_x_MainIlv,			//PT_BUF	io_px_MainIlv,
			&l_ul_ILVSize			//PUL		io_pul_MainIlvSize
			);

	if(l_i_Ret)
		return l_i_Ret;

//add pk ref
	l_i_Ret = MSO_SetPk (
			i_uc_NbPkRef,			//UC		i_uc_NbPk,
			i_px_PkRef,			//PT_BUF_PK	i_px_Pk,
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
	
	l_i_Ret = MSO_SendReceive(
				i_h_Mso100Handle,
				l_us_RequestId,			//US	i_us_I,		// Identifier
				l_auc_ILV_SendBuffer,		//PUC	i_puc_Frame,	// data to send
				l_ul_ILVSize,			//UL	i_ul_len,	// number of bytes to write
				&l_puc_ILV_ReceiveBuffer,	//PUC	*o_puc_dataOut,	// response buffer
				&l_ul_ILVSize,			//PUL	o_pul_len,	// response length
				&l_puc_V,			//PUC *o_ppuc_Data,	// ILV data 
				o_puc_ILV_Status,		//PUC	o_puc_ILV_Status,
				NULL);				//PUL	o_pul_InternalErrorCode)
	
	if (l_i_Ret || *o_puc_ILV_Status)
	{
		return l_i_Ret;
	}


	///////////////////////////////////////////
	// Analyse ILV reply
	///////////////////////////////////////////
	l_ul_Pos = 1;	//o_puc_ILV_Status
	
	//matching result
	l_i_Ret = ILV_GetValue(l_puc_ILV_ReceiveBuffer, &l_ul_Pos, (PUC)o_puc_MatchingResult, sizeof(*o_puc_MatchingResult));
	if (l_i_Ret)
	{
		return l_i_Ret;
	}
	
	//list refernce index : toujours retourné quelque soit le résultat du matching
	l_i_Ret = ILV_GetValue(l_puc_ILV_ReceiveBuffer, &l_ul_Pos, (PUC)o_puc_ListRefIndex, sizeof(*o_puc_ListRefIndex));
	if (l_i_Ret)
	{
		return l_i_Ret;
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

