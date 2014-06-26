#include <stdio.h>

#include "libMSO.h"
#include "MSO100.h"
#include "SpIlv.h"

/***********************************************************************
descriptions :

***********************************************************************/
I MSO_Cancel 
(
	MORPHO_HANDLE			i_h_Mso100Handle
)
{
	/* Variables declarations */
	I		l_i_Ret;
	UL		l_ul_ILVSize;
	US		l_us_RequestId;
	UC		l_auc_ILV_SendBuffer[256];


	l_us_RequestId = ILV_CANCEL;

	l_i_Ret = ILV_Init(l_auc_ILV_SendBuffer, &l_ul_ILVSize, l_us_RequestId);
	if (l_i_Ret != RETURN_NO_ERROR )
	{
		return l_i_Ret;
	}

	l_i_Ret = MSO_ComSend( i_h_Mso100Handle,
			       WRITE_TIMEOUT,		//UL		i_ul_Timeout,
			       l_auc_ILV_SendBuffer,	//PUC		i_puc_Data,
			       l_ul_ILVSize		//UL		i_ul_DataSize
			     );

	if (l_i_Ret)
	{
		return l_i_Ret;
	}

	return l_i_Ret;
}

