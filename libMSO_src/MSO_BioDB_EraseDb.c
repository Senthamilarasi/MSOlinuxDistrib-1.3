#include <stdio.h>

#include "libMSO.h"

#include "MSO100.h"
#include "SpIlv.h"


/***********************************************************************
descriptions :

	o_pul_EmbeddedError can be null.
***********************************************************************/
I MSO_BioDB_EraseDb 
(
	MORPHO_HANDLE	i_h_Mso100Handle,
	UC 		i_uc_IndexDB,
	PUC		o_puc_ILV_Status,
	PUL		o_pul_EmbeddedError
)
{
	US					l_us_RequestId;
	PUC					l_puc_ILV_ReceiveBuffer;
	I					l_i_Ret;
	UL					l_ul_ILVSize;
	T_BUF				l_x_ILV_SendBuffer;
	T_BUF				l_x_V;
	UC					l_auc_ILV_SendBuffer[ILV_SENDBUFFER_SIZE];

	*o_puc_ILV_Status	 = RETURN_NO_ERROR;

	if(o_pul_EmbeddedError)
		*o_pul_EmbeddedError = RETURN_NO_ERROR;

	l_us_RequestId		= ILV_ERASE_BASE;

	l_x_ILV_SendBuffer.m_ul_Size	= sizeof(l_auc_ILV_SendBuffer);
	l_x_ILV_SendBuffer.m_puc_Buf	= l_auc_ILV_SendBuffer;

	l_x_V.m_ul_Size = sizeof(UC);
	l_x_V.m_puc_Buf	= &i_uc_IndexDB;

	l_i_Ret = ILV_SetILV(
			   &l_x_ILV_SendBuffer, 
			   l_us_RequestId, 
			   &l_x_V,
			   &l_ul_ILVSize);

	if(l_i_Ret)
		return l_i_Ret;

//ILV request

	l_i_Ret = MSO_SendReceive(
				i_h_Mso100Handle,
				l_us_RequestId,				//US	i_us_I,			// Identifier
				l_auc_ILV_SendBuffer,		//PUC	i_puc_Frame,	// data to send
				l_ul_ILVSize,				//UL	i_ul_len,		// number of bytes to write
				&l_puc_ILV_ReceiveBuffer,	//PUC	*o_puc_dataOut,	// response buffer
				&l_ul_ILVSize,				//PUL	o_pul_len,		// response length
				NULL,						//PUC *o_ppuc_Data,		// ILV data 
				o_puc_ILV_Status,			//PUC	o_puc_ILV_Status,
				(PI)o_pul_EmbeddedError);		//PUL	o_pul_InternalErrorCode)
	
	if (l_i_Ret || *o_puc_ILV_Status)
	{
		return l_i_Ret;
	}

	MSO_FrameRelease(i_h_Mso100Handle,&l_puc_ILV_ReceiveBuffer);

	return l_i_Ret;
}

