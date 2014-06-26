#include <stdio.h>
#include <string.h>

#include "libMSO.h"

#include "MSO100.h"
#include "SpIlv.h"

/***********************************************************************
descriptions :

***********************************************************************/
I MSO_Bio_Verify 
(
	MORPHO_HANDLE		i_h_Mso100Handle,
	US			i_us_Timeout,
	US			i_us_MatchingTreshold,
	UC			i_uc_NbFinger,
	PT_BUF_PK		i_px_Pk,	
	UL			i_ul_AsynchronousEvent,
	PUC			o_puc_MatchingResult,
	PUL			o_pul_score,
	PUC			o_puc_ILV_Status
)
{
	US		l_us_RequestId;
	PUC		l_puc_ILV_ReceiveBuffer;
	UL		l_ul_ReceiveILVSize;
	I		l_i_Ret, i;
	UL		l_ul_IlvSize;
	T_BUF		l_x_ILV_SendBuffer, l_x_TempIlv;
	UC		l_auc_TempIlv[2048];
	T_BUF		l_x_V;
	UL		l_ul_Pos, l_ul_Timeout, l_ul_TempIlvSize;
	UC		l_uc_export_matching_score;
	UL		l_ul_ReceiveL;
	UC		l_uc_TempId;
	UC		l_auc_ILV_SendBuffer[ILV_SENDBUFFER_SIZE];

	/////////////////////////////////////////
	// Initialisations
	/////////////////////////////////////////
	
	*o_puc_ILV_Status	 = RETURN_NO_ERROR;

	/////////////////////////////////////////
	// ILV Building
	/////////////////////////////////////////

		l_us_RequestId		= ILV_AUTHENTICATE;

		l_x_ILV_SendBuffer.m_ul_Size	= sizeof(l_auc_ILV_SendBuffer);
		l_x_ILV_SendBuffer.m_puc_Buf	= l_auc_ILV_SendBuffer;

		//--------------------------------------
		// Set ILV
		//--------------------------------------

			//- - - - - - - - - - - - - - - - - 
			// Timeout
			//- - - - - - - - - - - - - - - - - 

				l_x_V.m_ul_Size = sizeof(US);
				l_x_V.m_puc_Buf	= (PUC) &i_us_Timeout;
				
				l_i_Ret = ILV_SetILV(
						   &l_x_ILV_SendBuffer, 
						   l_us_RequestId, 
						   &l_x_V,
						   &l_ul_IlvSize);

				if(l_i_Ret)
					return l_i_Ret;

			//- - - - - - - - - - - - - - - - - 
			// Matching Threshold
			//- - - - - - - - - - - - - - - - - 

				l_i_Ret = ILV_AddILV(
						   &l_x_ILV_SendBuffer,		//buffer
						   sizeof(US),			//taille des données à ajouter
						   (PUC) &i_us_MatchingTreshold,//données à ajouter
						   &l_ul_IlvSize);		//taille de l'ILV

				if(l_i_Ret)
					return l_i_Ret;

			//- - - - - - - - - - - - - - - - - 
			// Quality Threshold
			//- - - - - - - - - - - - - - - - - 

				i = 0;
				l_i_Ret = ILV_AddILV(
						   &l_x_ILV_SendBuffer,		//buffer
						   sizeof(UC),			//taille des données à ajouter
						   (PUC) &i,			//données à ajouter
						   &l_ul_IlvSize);		//taille de l'ILV

				if(l_i_Ret)
					return l_i_Ret;	

		//--------------------------------------
		// Additional Data Fields
		//--------------------------------------

			//- - - - - - - - - - - - - - - - 
			// Biometric Data 1 & 2
			//- - - - - - - - - - - - - - - - 
				l_i_Ret = MSO_SetPk (
						i_uc_NbFinger,		//UC		i_uc_NbPk,
						i_px_Pk,		//PT_BUF_PK	i_px_Pk,
						&l_x_ILV_SendBuffer,	//PT_BUF	io_px_MainIlv,
						&l_ul_IlvSize		//PUL		io_pul_MainIlvSize
						);

				if(l_i_Ret)
					return l_i_Ret;

			// Set a Temporary ILV

			l_x_TempIlv.m_puc_Buf = l_auc_TempIlv;
			l_x_TempIlv.m_ul_Size = sizeof(l_auc_TempIlv);

			l_x_V.m_ul_Size = sizeof(UL);
			l_x_V.m_puc_Buf = (PUC)&i_ul_AsynchronousEvent;
			
			l_i_Ret = ILV_SetILV(
				   &l_x_TempIlv, 
				   (US)ID_ASYNCHRONOUS_EVENT, 
				   &l_x_V,
				   &l_ul_TempIlvSize);

			if(l_i_Ret)
				return l_i_Ret;
		
			// Add the temporary ILV to the main ILV
			l_i_Ret = ILV_AddILV(
					   &l_x_ILV_SendBuffer,		//buffer
					   l_ul_TempIlvSize,		//taille des données à ajouter
					   l_auc_TempIlv,		//données à ajouter
					   &l_ul_IlvSize);		//taille de l'ILV

			if(l_i_Ret)
				return l_i_Ret;
			
			//add export matching score optional parameter
			if (o_pul_score!= NULL)
			{
				*o_pul_score=0;
				
				// Set a Temporary ILV
				l_uc_export_matching_score = TRUE;
				
				l_x_TempIlv.m_puc_Buf = l_auc_TempIlv;
				l_x_TempIlv.m_ul_Size = sizeof(l_auc_TempIlv);
				
				l_x_V.m_ul_Size = sizeof(l_uc_export_matching_score);
				l_x_V.m_puc_Buf = (PUC)&l_uc_export_matching_score;
				
				l_i_Ret = ILV_SetILV(
					&l_x_TempIlv, 
					(US)ID_MATCHING_SCORE, 
					&l_x_V,
					&l_ul_TempIlvSize);
				
				if(l_i_Ret)
					return l_i_Ret;
				
				// Add the temporary ILV to the main ILV
				l_i_Ret = ILV_AddILV(
					&l_x_ILV_SendBuffer,			//buffer
					l_ul_TempIlvSize,			//taille des données à ajouter
					l_auc_TempIlv,				//données à ajouter
					&l_ul_IlvSize);				//taille de l'ILV
				
				if(l_i_Ret)
					return l_i_Ret;
			}

	/////////////////////////////////////////
	// Send ILV request
	/////////////////////////////////////////

		if (i_us_Timeout == 0)
			l_ul_Timeout = INFINITE;
		else
			l_ul_Timeout = i_us_Timeout*1000 + WRITE_TIMEOUT;

		l_i_Ret = MSO_SendReceiveTimeout( i_h_Mso100Handle,
						  WRITE_TIMEOUT, 
						  l_ul_Timeout,
						  l_us_RequestId,
						  l_auc_ILV_SendBuffer, 
						  l_ul_IlvSize, 
						  &l_puc_ILV_ReceiveBuffer, 
						  &l_ul_ReceiveILVSize,
						  NULL,			//PUC *o_ppuc_Data,	//ILV data 
						  o_puc_ILV_Status,	//PUC	o_puc_ILV_Status,
						  NULL
						);
		
	if (!l_i_Ret)
	{
	
	///////////////////////////////////////////
	// Analyse ILV reply
	///////////////////////////////////////////	
		if(*o_puc_ILV_Status == 0)
		{
			l_ul_ReceiveL = ILV_GetL(l_puc_ILV_ReceiveBuffer);
			l_ul_Pos = 1; // '1' because Request Status already retrieved.			
			ILV_GetValue(l_puc_ILV_ReceiveBuffer, &l_ul_Pos, o_puc_MatchingResult, sizeof(UC));

			if ((o_pul_score!= NULL)&&(l_ul_Pos<l_ul_ReceiveL))
			{
				l_i_Ret = ILV_GetValue(l_puc_ILV_ReceiveBuffer, &l_ul_Pos, (PUC) &l_uc_TempId, sizeof(UC));
				if (l_i_Ret)
				{
					return l_i_Ret;
				}
				
				if (l_uc_TempId==ID_MATCHING_SCORE)
				{
					l_i_Ret = ILV_GetValue(l_puc_ILV_ReceiveBuffer, &l_ul_Pos, (PUC) &l_ul_IlvSize, sizeof(US));
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
		}
	}

	if(l_i_Ret)
	{
		MSO_Cancel(i_h_Mso100Handle);
	}

	MSO_FrameRelease(i_h_Mso100Handle,&l_puc_ILV_ReceiveBuffer);

	return l_i_Ret;
}

