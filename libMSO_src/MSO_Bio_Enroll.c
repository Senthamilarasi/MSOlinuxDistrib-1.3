#include <stdio.h>
#include <stdlib.h>

#include "libMSO.h"
#include "MSO100.h"

#include "SpIlv.h"


I	MSO_GetExportImage
(
	PT_EXPORT_IMAGE		io_px_ExportImage,
	UC			i_uc_NbFinger,
	PUC			i_puc_ILV_ReceiveBuffer,
	PUL			io_pul_Pos
)
{
	UC	l_uc_Cnt;
	I	l_i_Ret;
	PUC	l_puc_TempILV;
	US	l_us_I;
	UL	l_ul_L;
	PUC	l_puc_V;

	l_i_Ret = RETURN_NO_ERROR;
	
	if(io_px_ExportImage)
	{
		for(l_uc_Cnt = 0; l_uc_Cnt < i_uc_NbFinger; l_uc_Cnt++)
		{
			if (!l_i_Ret)
			{
				l_puc_TempILV = i_puc_ILV_ReceiveBuffer + *io_pul_Pos + SizeOfIandL(i_puc_ILV_ReceiveBuffer);
				l_i_Ret = ILV_GetILV(
						   l_puc_TempILV,	//PUC	i_puc_ILV, 
						   &l_us_I,		//PUS	o_pul_I, 
						   &l_ul_L,		//PUL	o_pul_L, 
						   &l_puc_V		//PUC*	o_ppuc_V);
						   );
			}

			if (!l_i_Ret)
			{
				if(l_us_I != ID_EXPORT_IMAGE)
					l_i_Ret = ERROR_APPLI_BAD_I_VALUE;
			}
			if (!l_i_Ret)
			{
				if(io_px_ExportImage->io_px_BufImage->m_puc_Buf == NULL)
				{
					io_px_ExportImage->io_px_BufImage->m_puc_Buf = (PUC)malloc ( l_ul_L );
				}
				else if(l_ul_L > io_px_ExportImage->io_px_BufImage[l_uc_Cnt].m_ul_Size)
					l_i_Ret = ILV_ERROR_NOT_ENOUGH_MEMORY;
			}
			if (!l_i_Ret)
			{
				*io_pul_Pos = *io_pul_Pos + SizeOfIandL(l_puc_TempILV);

				io_px_ExportImage->io_px_BufImage[l_uc_Cnt].m_ul_Size = l_ul_L; 

				l_i_Ret = ILV_GetValue(
						i_puc_ILV_ReceiveBuffer, 
						io_pul_Pos, 
						io_px_ExportImage->io_px_BufImage[l_uc_Cnt].m_puc_Buf, 
						io_px_ExportImage->io_px_BufImage[l_uc_Cnt].m_ul_Size
						);
			}
		}
	}

	return l_i_Ret;
}

I MSO_Bio_Enroll 
(
	MORPHO_HANDLE	i_h_Mso100Handle,
	UC 				i_uc_IndexDB,
	US				i_us_Timeout,
	UC				i_uc_EnrollmentType,
	UC				i_uc_NbFinger,
	UC				i_uc_SaveRecord,
	UC				i_uc_NormalizedPK_Type,
	UC				i_uc_NbAddField,
	PT_BUF			i_px_AddField,
	UL				i_ul_AsynchronousEvent,
	PT_EXPORT_PK	io_px_ExportPk,				//can be NULL
	PT_EXPORT_IMAGE	io_px_ExportImage,			//can be NULL
	PUC				o_puc_EnrollStatus,
	PUL				o_pul_UserDBIndex,
	PUC				o_puc_ILV_Status
)
{

	US		l_us_RequestId;
	US		l_us_TempI;
	PUC		l_puc_ILV_ReceiveBuffer;
	UL		l_ul_ReceiveILVSize;
	I		l_i_Ret, i;
	UL		l_ul_IlvSize, l_ul_TempIlvSize;
	UC		l_auc_TempIlv[256];
	T_BUF		l_x_ILV_SendBuffer;
	T_BUF		l_x_V;
	T_BUF		l_x_bufField;
	T_BUF		l_x_TempIlv;
	UL		l_ul_Pos;
	UL		l_ul_Timeout;

	UC		l_auc_TempIlv2[256];
	T_BUF		l_x_TempIlv2;
	UL		l_ul_TempIlvSize2;

	UC		l_uc_RFU;
	UC		l_uc_ExportMinutiae;

	UC		l_auc_ILV_SendBuffer[ILV_SENDBUFFER_SIZE];
	
	/////////////////////////////////////////
	// Initialisations
	/////////////////////////////////////////
	
	*o_puc_ILV_Status	 = RETURN_NO_ERROR;

	/////////////////////////////////////////
	// ILV Building
	/////////////////////////////////////////

	l_us_RequestId		= ILV_ENROLL;

	l_x_ILV_SendBuffer.m_ul_Size	= sizeof(l_auc_ILV_SendBuffer);
	l_x_ILV_SendBuffer.m_puc_Buf	= l_auc_ILV_SendBuffer;

		//--------------------------------------
		// Set ILV
		//--------------------------------------

			//- - - - - - - - - - - - - - - - - 
			// ID DB
			//- - - - - - - - - - - - - - - - - 

				l_x_V.m_ul_Size = sizeof(UC);
				l_x_V.m_puc_Buf	= &i_uc_IndexDB;
				
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
						   sizeof(US),				//taille des donn�es � ajouter
						   (PUC) &i_us_Timeout,		//donn�es � ajouter
						   &l_ul_IlvSize);			//taille de l'ILV

				if(l_i_Ret)
					return l_i_Ret;

			//- - - - - - - - - - - - - - - - - 
			// Quality Threshold
			//- - - - - - - - - - - - - - - - - 

				i = 0;
				l_i_Ret = ILV_AddILV(
						   &l_x_ILV_SendBuffer,	//buffer
						   sizeof(UC),		//taille des donn�es � ajouter
						   (PUC) &i,		//donn�es � ajouter
						   &l_ul_IlvSize);	//taille de l'ILV

				if(l_i_Ret)
					return l_i_Ret;

			//- - - - - - - - - - - - - - - - - 
			// Enrollment Type
			//- - - - - - - - - - - - - - - - - 

				l_i_Ret = ILV_AddILV(
						   &l_x_ILV_SendBuffer,	//buffer
						   sizeof(UC),		//taille des donn�es � ajouter
						   &i_uc_EnrollmentType,//donn�es � ajouter
						   &l_ul_IlvSize);	//taille de l'ILV

				if(l_i_Ret)
					return l_i_Ret;

			//- - - - - - - - - - - - - - - - - 
			// Nb Finger
			//- - - - - - - - - - - - - - - - - 
				
				l_i_Ret = ILV_AddILV(
						   &l_x_ILV_SendBuffer,	//buffer
						   sizeof(UC),		//taille des donn�es � ajouter
						   &i_uc_NbFinger,	//donn�es � ajouter
						   &l_ul_IlvSize);	//taille de l'ILV

				if(l_i_Ret)
					return l_i_Ret;		

			//- - - - - - - - - - - - - - - - - 
			// SaveRecord
			//- - - - - - - - - - - - - - - - - 

				l_i_Ret = ILV_AddILV(
						   &l_x_ILV_SendBuffer,	//buffer
						   sizeof(UC),		//taille des donn�es � ajouter
						   &i_uc_SaveRecord,	//donn�es � ajouter
						   &l_ul_IlvSize);	//taille de l'ILV

				if(l_i_Ret)
					return l_i_Ret;

			//- - - - - - - - - - - - - - - - - 
			// ExportMinutiae
			//- - - - - - - - - - - - - - - - - 

				if(io_px_ExportPk)
				{
					l_uc_ExportMinutiae = io_px_ExportPk->i_uc_ExportMinutiae;
				}
				else
				{
					l_uc_ExportMinutiae = 0;
				}

				
				l_i_Ret = ILV_AddILV(
						   &l_x_ILV_SendBuffer,	//buffer
						   sizeof(UC),		//taille des donn�es � ajouter
						   &l_uc_ExportMinutiae,//donn�es � ajouter
						   &l_ul_IlvSize);	//taille de l'ILV

				if(l_i_Ret)
					return l_i_Ret;

		//--------------------------------------
		// Additional Data Fields
		//--------------------------------------
			
			l_x_TempIlv.m_puc_Buf = l_auc_TempIlv;

			for( i=0; i<i_uc_NbAddField; i++)
			{			
				//- - - - - - - - - - - - - - - - - 
				// Set a Temporary ILV
				//- - - - - - - - - - - - - - - - - 
				
					if(i==0)	l_us_TempI = ID_USER_ID; // !!!!! We assume the First Additional Data Field is the User ID
					else		l_us_TempI = ID_PUC_DATA;

					l_x_TempIlv.m_ul_Size = 256;

					l_x_bufField.m_ul_Size = i_px_AddField[i].m_ul_Size;
					l_x_bufField.m_puc_Buf = i_px_AddField[i].m_puc_Buf;
					
					l_i_Ret = ILV_SetILV(
						   &l_x_TempIlv, 
						   l_us_TempI, 
						   &l_x_bufField,
						   &l_ul_TempIlvSize);

					if(l_i_Ret)
						return l_i_Ret;
				
				//- - - - - - - - - - - - - - - - - - - - -  
				// Add the temporary ILV to the main ILV
				//- - - - - - - - - - - - - - - - - - - - -  

					l_i_Ret = ILV_AddILV(
							   &l_x_ILV_SendBuffer,	//buffer
							   l_ul_TempIlvSize,	//taille des donn�es � ajouter
							   l_auc_TempIlv,	//donn�es � ajouter
							   &l_ul_IlvSize);	//taille de l'ILV

					if(l_i_Ret)
						return l_i_Ret;
			}

			if(i_ul_AsynchronousEvent)
			{
				//- - - - - - - - - - - - - - - - - 
				// Asynchronous events
				//- - - - - - - - - - - - - - - - - 

				// Set a Temporary ILV

				l_x_TempIlv.m_puc_Buf = l_auc_TempIlv;
				l_x_TempIlv.m_ul_Size = sizeof(l_auc_TempIlv);

				l_us_TempI = ID_ASYNCHRONOUS_EVENT;

				l_x_V.m_ul_Size = sizeof(UL);
				l_x_V.m_puc_Buf = (PUC)&i_ul_AsynchronousEvent;
				
				l_i_Ret = ILV_SetILV(
					   &l_x_TempIlv, 
					   l_us_TempI, 
					   &l_x_V,
					   &l_ul_TempIlvSize);

				if(l_i_Ret)
					return l_i_Ret;
			
				// Add the temporary ILV to the main ILV
				l_i_Ret = ILV_AddILV(
						   &l_x_ILV_SendBuffer,	//buffer
						   l_ul_TempIlvSize,	//taille des donn�es � ajouter
						   l_auc_TempIlv,	//donn�es � ajouter
						   &l_ul_IlvSize);	//taille de l'ILV

				if(l_i_Ret)
					return l_i_Ret;
			}

			//- - - - - - - - - - - - - - - - - 
			// Biometric algo parameters
			//- - - - - - - - - - - - - - - - - 

			// Set a Temporary ILV
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
						   &l_x_ILV_SendBuffer,	//buffer
						   l_ul_TempIlvSize,	//taille des donn�es � ajouter
						   l_auc_TempIlv,	//donn�es � ajouter
						   &l_ul_IlvSize);	//taille de l'ILV

				if(l_i_Ret)
					return l_i_Ret;
			}

			if(io_px_ExportImage)
			{
				{
					UC l_auc_temp[2];
					UC l_uc_size;

					l_x_TempIlv2.m_puc_Buf = l_auc_TempIlv2;
					l_x_TempIlv2.m_ul_Size = sizeof(l_auc_TempIlv2);
					l_us_TempI = ID_COMPRESSION;

					l_auc_temp[0] = io_px_ExportImage->i_uc_CompressionType;
					l_uc_size = 1;
					if(io_px_ExportImage->i_uc_CompressionType == ID_COMPRESSION_WSQ)
					{
						l_auc_temp[1] = io_px_ExportImage->i_uc_CompressionParam;
						l_uc_size ++;
					}

					l_x_V.m_ul_Size = l_uc_size;
					l_x_V.m_puc_Buf = (PUC)l_auc_temp;
					
					l_i_Ret = ILV_SetILV(
						   &l_x_TempIlv2, 
						   l_us_TempI, 
						   &l_x_V,
						   &l_ul_TempIlvSize2);

					if(l_i_Ret)
						return l_i_Ret;

					l_uc_RFU = 0;

					l_i_Ret = ILV_AddILV(
							   &l_x_TempIlv2,//buffer
							   sizeof(UC),	//taille des donn�es � ajouter
							   &l_uc_RFU,	//donn�es � ajouter
							   &l_ul_TempIlvSize2);	//taille de l'ILV

					if(l_i_Ret)
						return l_i_Ret;
				}
				{
					l_x_TempIlv.m_puc_Buf = l_auc_TempIlv;
					l_x_TempIlv.m_ul_Size = sizeof(l_auc_TempIlv);

					l_us_TempI = ID_EXPORT_IMAGE;

					l_x_V.m_ul_Size = sizeof(io_px_ExportImage->i_uc_ExportImageType);
					l_x_V.m_puc_Buf = &(io_px_ExportImage->i_uc_ExportImageType);
					
					l_i_Ret = ILV_SetILV(
						   &l_x_TempIlv, 
						   l_us_TempI, 
						   &l_x_V,
						   &l_ul_TempIlvSize);

					if(l_i_Ret)
						return l_i_Ret;		

					l_i_Ret = ILV_AddILV(
							   &l_x_TempIlv,	//buffer
							   l_ul_TempIlvSize2,	//taille des donn�es � ajouter
							   l_x_TempIlv2.m_puc_Buf,	//donn�es � ajouter
							   &l_ul_TempIlvSize);	//taille de l'ILV

					if(l_i_Ret)
						return l_i_Ret;
				}
			
				// Add the temporary ILV to the main ILV
				l_i_Ret = ILV_AddILV(
						   &l_x_ILV_SendBuffer,		//buffer
						   l_ul_TempIlvSize,		//taille des donn�es � ajouter
						   l_auc_TempIlv,		//donn�es � ajouter
						   &l_ul_IlvSize);		//taille de l'ILV

				if(l_i_Ret)
					return l_i_Ret;
			}

	/////////////////////////////////////////
	// Send ILV request
	/////////////////////////////////////////

		if (i_us_Timeout == 0)
			l_ul_Timeout = INFINITE;
		else
		{
			l_ul_Timeout = i_us_Timeout*1000 + WRITE_TIMEOUT;

			if(i_uc_SaveRecord)
				l_ul_Timeout += IDENT_TIMEOUT;
		}

		l_i_Ret = MSO_SendReceiveTimeout(
				i_h_Mso100Handle,
				WRITE_TIMEOUT,			//UL	i_ul_TransmitTimeout,
				l_ul_Timeout,			//UL	i_ul_ReceiveTimeout,	// reception timeout (ms)
				l_us_RequestId,			//US	i_us_I,			// Identifier
				l_auc_ILV_SendBuffer,		//PUC	i_puc_Frame,	// data to send
				l_ul_IlvSize,			//UL	i_ul_len,	// number of bytes to write
				&l_puc_ILV_ReceiveBuffer,	//PUC	*o_ppuc_dataOut,	// response buffer
				&l_ul_ReceiveILVSize,		//PUL	o_pul_len,	// response length
				NULL,				//PUC *o_ppuc_Data,			// ILV data 
				o_puc_ILV_Status,		//PUC	o_puc_ILV_Status,
				NULL);				//PUL	o_pul_InternalErrorCode)
		
	if (!l_i_Ret)
	{
		
	///////////////////////////////////////////
	// Analyse ILV reply
	///////////////////////////////////////////
		
		if(*o_puc_ILV_Status == 0)
		{
			l_ul_Pos = 1;

			ILV_GetValue(l_puc_ILV_ReceiveBuffer, &l_ul_Pos, o_puc_EnrollStatus, sizeof(UC));
			
			if(*o_puc_EnrollStatus == ILV_OK )
			{
				ILV_GetValue(l_puc_ILV_ReceiveBuffer, &l_ul_Pos, (PUC) o_pul_UserDBIndex, sizeof(UL));

				if(io_px_ExportPk && io_px_ExportPk->i_uc_ExportMinutiae)
				{
					MSO_GetPk (i_uc_NbFinger, l_puc_ILV_ReceiveBuffer, &l_ul_Pos, io_px_ExportPk);
				}
				if (!l_i_Ret)
				{
					l_i_Ret = MSO_GetExportImage(
								io_px_ExportImage,//PT_EXPORT_IMAGE io_px_ExportImage,
								i_uc_NbFinger,	//UC	i_uc_NbFinger,
								l_puc_ILV_ReceiveBuffer,//PUC	i_puc_ILV_ReceiveBuffer,
								&l_ul_Pos	//PUL	io_pul_Pos
								);
				}
			}
			else if ((*o_puc_EnrollStatus == ILVSTS_FFD )||(*o_puc_EnrollStatus == ILVSTS_MOIST_FINGER ))
			{
				if (!l_i_Ret)
				{
					l_i_Ret = MSO_GetExportImage(
								io_px_ExportImage,//PT_EXPORT_IMAGE io_px_ExportImage,
								i_uc_NbFinger,	//UC	i_uc_NbFinger,
								l_puc_ILV_ReceiveBuffer,//PUC	i_puc_ILV_ReceiveBuffer,
								&l_ul_Pos	//PUL	io_pul_Pos
								);				
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

