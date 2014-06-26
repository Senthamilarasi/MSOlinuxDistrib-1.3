#include <stdio.h>
#include <string.h>

#include "libMSO.h"
#include "MSO100.h"
#include "SpIlv.h"

//--------------------------------------------------------------------
//
//  Function:   MSO_GetDesciptor
//----------------------------------------------------------------------------
I MSO_GetDescriptor
(
	MORPHO_HANDLE	i_h_Mso100Handle,
	UC		i_uc_DescFormat,
	PUC		o_puc_ILV_Status,
	VOID*		i_pv_DescProduct,
	UL		i_ul_SizeOfDescProduct,
	VOID*		i_pv_DescSensor,
	UL		i_ul_SizeOfDescSensor,
	VOID*		i_pv_DescSoftware,
	UL		i_ul_SizeOfDescSoftware
)
{
	/* Variables declarations */
	I		l_i_Ret;
	UL		l_ul_ILVSize;
	US		l_us_RequestId;
	UL		l_ul_L;
	UC*		l_puc_V;
	US		l_us_I_Desc;
	UL		l_ul_L_Desc;
	UC*		l_puc_V_Desc;
	PUC		l_puc_ILV_ReceiveBuffer;
	UC		l_auc_ILV_SendBuffer[256];

	*o_puc_ILV_Status = RETURN_NO_ERROR;

	if(i_pv_DescProduct)
		memset(i_pv_DescProduct,0,i_ul_SizeOfDescProduct);

	if(i_pv_DescSensor)
		memset(i_pv_DescSensor,0,i_ul_SizeOfDescSensor);

	if(i_pv_DescSoftware)
		memset(i_pv_DescSoftware,0,i_ul_SizeOfDescSoftware);
	
	l_us_RequestId = ILV_GET_DESCRIPTOR;

	l_i_Ret = ILV_Init(l_auc_ILV_SendBuffer, &l_ul_ILVSize, l_us_RequestId);
	if (l_i_Ret<0)
	{
		return l_i_Ret;
	}

	l_i_Ret = ILV_AddValue(l_auc_ILV_SendBuffer, &l_ul_ILVSize, &i_uc_DescFormat,sizeof(UC));
	if (l_i_Ret<0)
	{
		return l_i_Ret;
	}

	l_i_Ret = MSO_SendReceive( i_h_Mso100Handle,
				   l_us_RequestId,		//US	i_us_I,		// Identifier
				   l_auc_ILV_SendBuffer,	//PUC	i_puc_Frame,	// data to send
				   l_ul_ILVSize,		//UL	i_ul_len,	// number of bytes to write
				   &l_puc_ILV_ReceiveBuffer,	//PUC	*o_puc_dataOut,	// response buffer
				   &l_ul_ILVSize,		//PUL	o_pul_len,		// response length
				   &l_puc_V,			//PUC *o_ppuc_Data,		// ILV data 
				   o_puc_ILV_Status,		//PUC	o_puc_ILV_Status,
				   NULL
				 );
	
	if ( l_i_Ret != RETURN_NO_ERROR || *o_puc_ILV_Status != ILV_OK)
	{
	//		printf("MSO_SendReceive ret 0x%08X\n",l_i_Ret);
	}
	else
	{
		l_ul_L = ILV_GetL(l_puc_ILV_ReceiveBuffer);
		l_ul_L-- ;// pour le Request Status

		while (l_ul_L!=0)
		{
			l_us_I_Desc = ILV_GetI(l_puc_V);
			l_ul_L_Desc = ILV_GetL(l_puc_V);
			l_puc_V_Desc = ILV_GetV(l_puc_V);
							
			switch(l_us_I_Desc)
			{
			case ID_DESC_PRODUCT:
				if(i_pv_DescProduct)
					memcpy((PUC)i_pv_DescProduct,l_puc_V_Desc, min ( i_ul_SizeOfDescProduct, l_ul_L_Desc ));
				break;
			case ID_DESC_SOFTWARE:
				if(i_pv_DescSoftware)
					memcpy((PUC)i_pv_DescSoftware,l_puc_V_Desc, min ( i_ul_SizeOfDescSoftware, l_ul_L_Desc ));
				break;
			case ID_DESC_SENSOR:
				if(i_pv_DescSensor)
					memcpy((PUC)i_pv_DescSensor,l_puc_V_Desc, min ( i_ul_SizeOfDescSensor, l_ul_L_Desc ));
				break;
			default:
	//			printf ("Unknown Desc ID 0x%04X\n",l_us_I_Desc);
				break;
			}
			l_ul_L_Desc = ILV_GetSize(l_puc_V);
			if (l_ul_L_Desc>l_ul_L)
				l_ul_L=0;	//exit
			else
			{
				l_ul_L-=l_ul_L_Desc;
				l_puc_V+=l_ul_L_Desc;
			}
		}
		l_i_Ret = RETURN_NO_ERROR;

		MSO_FrameRelease(i_h_Mso100Handle,&l_puc_ILV_ReceiveBuffer);
	}

	return l_i_Ret;
}

