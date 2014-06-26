#include <stdio.h>
#include <string.h>

#include "libMSO.h"
#include "MSO100.h"

#include "SpIlv.h"


I MSO_Bio_Identify
(
	MORPHO_HANDLE	i_h_Mso100Handle,
	UC 		i_uc_UidDB,
	US		i_us_Timeout,
	US		i_us_MatchingTreshold,
	UL		i_ul_AsynchronousEvent,
	PUC		o_puc_MatchingResult,
	PUL		o_pul_UserDBIndex,
	PT_BUF		o_px_UserID,
	PUL		io_pul_AddFieldNb,
	PT_BUF		o_px_AddFieldValue,
	PUL		o_pul_score,
	PUC		o_puc_ILV_Status
)
{
	US		l_us_RequestId;
	PUC		l_puc_ILV_ReceiveBuffer;
	UL		l_ul_ReceiveILVSize;
	I		l_i_Ret, i;
	UL		l_ul_IlvSize;
	US		l_us_IlvSize;
	T_BUF		l_x_ILV_SendBuffer;
	T_BUF		l_x_V;
	T_BUF		l_x_TempIlv;
	UC		l_auc_TempIlv[20];
	UL		l_ul_Pos, l_ul_Timeout, l_ul_TempIlvSize;
	UC		l_uc_ID;
	UL		l_ul_AddFieldNb;
	UL		l_ul_ReceiveL;
	UC		l_uc_export_matching_score;
	UC		l_auc_ILV_SendBuffer[256];
	
	/////////////////////////////////////////
	// Initialisations
	/////////////////////////////////////////
	
		*o_puc_ILV_Status	 = RETURN_NO_ERROR;
		o_px_UserID->m_ul_Size = 0;
		*o_pul_UserDBIndex = 0xFFFFFFFF;
		l_ul_AddFieldNb = 0;

	/////////////////////////////////////////
	// ILV Building
	/////////////////////////////////////////

		l_us_RequestId		= ILV_IDENTIFY;

		l_x_ILV_SendBuffer.m_ul_Size	= sizeof(l_auc_ILV_SendBuffer);
		l_x_ILV_SendBuffer.m_puc_Buf	= l_auc_ILV_SendBuffer;

		//--------------------------------------
		// Set ILV
		//--------------------------------------

			//- - - - - - - - - - - - - - - - - 
			// ID DB
			//- - - - - - - - - - - - - - - - - 

				l_x_V.m_ul_Size = sizeof(UC);
				l_x_V.m_puc_Buf	= &i_uc_UidDB;
				
				l_i_Ret = ILV_SetILV(
						   &l_x_ILV_SendBuffer, 
						   l_us_RequestId, 
						   &l_x_V,
						   &l_ul_IlvSize);

				if(l_i_Ret)
					return l_i_Ret;

			//- - - - - - - - - - - - - - - - - 
			// Timeout
			//- - - - - - - - - - - - - - - - - 

				l_i_Ret = ILV_AddILV(
						   &l_x_ILV_SendBuffer,		//buffer
						   sizeof(US),				//taille des données à ajouter
						   (PUC) &i_us_Timeout,		//données à ajouter
						   &l_ul_IlvSize);			//taille de l'ILV

				if(l_i_Ret)
					return l_i_Ret;
	
			//- - - - - - - - - - - - - - - - - 
			// Matching Threshold
			//- - - - - - - - - - - - - - - - - 

				l_i_Ret = ILV_AddILV(
						   &l_x_ILV_SendBuffer,		//buffer
						   sizeof(US),				//taille des données à ajouter
						   (PUC) &i_us_MatchingTreshold,	//données à ajouter
						   &l_ul_IlvSize);			//taille de l'ILV

				if(l_i_Ret)
					return l_i_Ret;

			//- - - - - - - - - - - - - - - - - 
			// Quality Threshold
			//- - - - - - - - - - - - - - - - - 

				i = 0;
				l_i_Ret = ILV_AddILV(
						   &l_x_ILV_SendBuffer,		//buffer
						   sizeof(UC),				//taille des données à ajouter
						   (PUC) &i,				//données à ajouter
						   &l_ul_IlvSize);			//taille de l'ILV

				if(l_i_Ret)
					return l_i_Ret;

		//--------------------------------------
		// Additional Data Fields
		//--------------------------------------

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
					   &l_x_ILV_SendBuffer,			//buffer
					   l_ul_TempIlvSize,			//taille des données à ajouter
					   l_auc_TempIlv,				//données à ajouter
					   &l_ul_IlvSize);				//taille de l'ILV

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
			l_ul_Timeout = i_us_Timeout*1000 + WRITE_TIMEOUT + IDENT_TIMEOUT;

		l_i_Ret = MSO_SendReceiveTimeout( i_h_Mso100Handle,
						  WRITE_TIMEOUT, 
						  l_ul_Timeout,
						  l_us_RequestId,
						  l_auc_ILV_SendBuffer, 
						  l_ul_IlvSize, 
						  &l_puc_ILV_ReceiveBuffer, 
						  &l_ul_ReceiveILVSize,
						  NULL,			//PUC *o_ppuc_Data,	// ILV data 
						  o_puc_ILV_Status,	//PUC	o_puc_ILV_Status,
						  NULL
						);
				
	if (!l_i_Ret)
	{
		l_ul_Pos = 1; // '1' because Request Status already retrieved.
		if(*o_puc_ILV_Status == 0)
		{
			l_ul_ReceiveL = ILV_GetL(l_puc_ILV_ReceiveBuffer);
			//get matching result
			ILV_GetValue(l_puc_ILV_ReceiveBuffer, &l_ul_Pos, o_puc_MatchingResult, sizeof(UC));

			if (*o_puc_MatchingResult ==  ILVSTS_HIT)
			{
				// retrieve base index
				l_i_Ret = ILV_GetValue(l_puc_ILV_ReceiveBuffer, &l_ul_Pos, (PUC) o_pul_UserDBIndex, sizeof(*o_pul_UserDBIndex));
				if (l_i_Ret)
				{
					return l_i_Ret;
				}
				// retrieve ILV user index
				l_i_Ret = ILV_GetValue(l_puc_ILV_ReceiveBuffer, &l_ul_Pos, (PUC) &l_uc_ID, sizeof(l_uc_ID));
				if (l_i_Ret)
				{
					return l_i_Ret;
				}
				if (l_uc_ID == ID_USER_ID)
				{
					l_i_Ret = ILV_GetValue(l_puc_ILV_ReceiveBuffer, &l_ul_Pos, (PUC) &o_px_UserID->m_ul_Size, sizeof(US));
					if (l_i_Ret)
					{
						return l_i_Ret;
					}
					l_i_Ret = ILV_GetValue(l_puc_ILV_ReceiveBuffer, &l_ul_Pos, o_px_UserID->m_puc_Buf, o_px_UserID->m_ul_Size);
					if (l_i_Ret)
					{
						return l_i_Ret;
					}
				}

				//retrieve additional fields value
				l_ul_AddFieldNb = 0;
				while (l_ul_Pos<l_ul_ReceiveL)
				{
					l_i_Ret = ILV_GetValue(l_puc_ILV_ReceiveBuffer, &l_ul_Pos, (PUC) &l_uc_ID, sizeof(l_uc_ID));
					if (l_i_Ret)
					{
						return l_i_Ret;
					}
					if ((l_uc_ID == ID_PUC_DATA)&&(l_ul_AddFieldNb<*io_pul_AddFieldNb))
					{
						l_i_Ret = ILV_GetValue(l_puc_ILV_ReceiveBuffer, &l_ul_Pos, (PUC) &o_px_AddFieldValue[l_ul_AddFieldNb].m_ul_Size, sizeof(US));
						if (l_i_Ret)
						{
							return l_i_Ret;
						}
						l_i_Ret = ILV_GetValue(l_puc_ILV_ReceiveBuffer, &l_ul_Pos, o_px_AddFieldValue[l_ul_AddFieldNb].m_puc_Buf, o_px_AddFieldValue[l_ul_AddFieldNb].m_ul_Size);
						if (l_i_Ret)
						{
							return l_i_Ret;
						}

						l_ul_AddFieldNb++;
					}

					if ((o_pul_score!= NULL)&&(l_uc_ID == ID_MATCHING_SCORE))
					{
						l_i_Ret = ILV_GetValue(l_puc_ILV_ReceiveBuffer, &l_ul_Pos, (PUC) &l_us_IlvSize, sizeof(US));
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
				*io_pul_AddFieldNb=l_ul_AddFieldNb;
			}
			else
			{
				o_px_UserID->m_ul_Size = 0;
				*o_pul_UserDBIndex = 0;					

				while (l_ul_Pos<l_ul_ReceiveL)
				{
					if (o_pul_score!= NULL)
					{
						l_i_Ret = ILV_GetValue(l_puc_ILV_ReceiveBuffer, &l_ul_Pos, (PUC) &l_uc_ID, sizeof(l_uc_ID));
						if (l_i_Ret)
						{
							return l_i_Ret;
						}

						if (l_uc_ID==ID_MATCHING_SCORE)
						{
							l_i_Ret = ILV_GetValue(l_puc_ILV_ReceiveBuffer, &l_ul_Pos, (PUC) &l_us_IlvSize, sizeof(US));
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
		}
	}
	if(l_i_Ret)
	{
		MSO_Cancel(i_h_Mso100Handle);
	}
	else
	{
		l_i_Ret = *o_puc_ILV_Status;
	}

	MSO_FrameRelease(i_h_Mso100Handle,&l_puc_ILV_ReceiveBuffer);

	return l_i_Ret;
}

