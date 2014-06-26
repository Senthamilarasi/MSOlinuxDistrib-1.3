#include <stdio.h>
#include <string.h>

#include "libMSO.h"
#include "MSO100.h"

#include "SpIlv.h"

/***********************************************************************
descriptions :

***********************************************************************/

I MSO_SECU_GetSerialNumber
(
	MORPHO_HANDLE	i_h_Mso100Handle,
	UC		o_auc_SerialNumber[MSO_SERIAL_NUMBER_LEN],
	PUC		o_puc_SecuConfig,
	PUS		o_pus_SecuMaxFAR,
	PUC		o_puc_ILV_Status
)
{
	/* Variables declarations */
	I		l_i_Ret;
	UL		l_ul_ILVSize;
	US		l_us_RequestId;
	PUC		l_puc_ILV_ReceiveBuffer;
	PUC		l_puc_V;
	UC		l_auc_ILV_SendBuffer[256];
	
	if(i_h_Mso100Handle == NULL)
	{
		return MSO_BAD_PARAMETER;
	}

	l_ul_ILVSize = 0;
	l_puc_ILV_ReceiveBuffer = NULL;
	l_puc_V = NULL;
	memset(l_auc_ILV_SendBuffer,0,sizeof(l_auc_ILV_SendBuffer));
	
	if(o_auc_SerialNumber)
	{
		memset(o_auc_SerialNumber,0, MSO_SERIAL_NUMBER_LEN);
	}
	if(o_puc_SecuConfig)
	{
		*o_puc_SecuConfig = 0;
	}
	if(o_pus_SecuMaxFAR)
	{
		*o_pus_SecuMaxFAR = 0;
	}

	*o_puc_ILV_Status = RETURN_NO_ERROR;
	
	l_us_RequestId = ILV_SECU_GET_CONFIG;

	l_i_Ret = ILV_Init( l_auc_ILV_SendBuffer, &l_ul_ILVSize, l_us_RequestId);
	if (l_i_Ret<0)
	{
		return l_i_Ret;
	}

//ILV request
	l_i_Ret = MSO_SendReceive( i_h_Mso100Handle,
				   l_us_RequestId,		//US	i_us_I,		// Identifier
				   l_auc_ILV_SendBuffer,	//PUC	i_puc_Frame,	// data to send
				   l_ul_ILVSize,		//UL	i_ul_len,	// number of bytes to write
				   &l_puc_ILV_ReceiveBuffer,	//PUC	*o_puc_dataOut,	// response buffer
				   &l_ul_ILVSize,		//PUL	o_pul_len,	// response length
				   &l_puc_V,			//PUC	*o_ppuc_Data,	// ILV data 
				   o_puc_ILV_Status,		//PUC	o_puc_ILV_Status,
				   NULL
				 );
	
	if (l_i_Ret || *o_puc_ILV_Status)
	{
//		printf("MSO_SendReceive ret 0x%08X\n",l_i_Ret);
	}
	else
	{
		if(o_auc_SerialNumber)
		{
			memcpy(o_auc_SerialNumber, l_puc_V, MSO_SERIAL_NUMBER_LEN);
		}
		if(o_puc_SecuConfig)
		{
			*o_puc_SecuConfig = l_puc_V[MSO_SERIAL_NUMBER_LEN];
		}
		if(o_pus_SecuMaxFAR)
		{
			memcpy( (PUC)o_pus_SecuMaxFAR, l_puc_V+MSO_SERIAL_NUMBER_LEN+sizeof(UC), sizeof(US) );
		}
	}

	MSO_FrameRelease(i_h_Mso100Handle,&l_puc_ILV_ReceiveBuffer);

	return l_i_Ret;
}

