#include <stdio.h>
#include <stdlib.h>

#include "libMSO.h"
#include "MSO100.h"
#include "SpIlv.h"

#include "sp_rs232.h"

/***********************************************************************
descriptions :

***********************************************************************/
I MSO_ConfigUart 
(
	MORPHO_HANDLE	i_h_Mso100Handle,
	UL		i_ul_BaudRate,
	UC		i_uc_ByteSize,
	UC		i_uc_StopBits,
	UC		i_uc_Parity,
	UC		i_uc_FlowCtrl,
	UC		i_uc_Send,
	UC		i_uc_Format,
	PUC		o_puc_ILV_Status
)
{
	US			l_us_RequestId;
	PUC			l_puc_ILV_ReceiveBuffer;
	I			l_i_Ret;
	UL			l_ul_ILVSize;
	T_BUF			l_x_ILV_SendBuffer;
	T_BUF			l_x_V;
	T_BUF			l_x_ILV_Buffer;
	T_ILV_UART_CONFIG	l_x_IlvUartConfig;
	
	l_us_RequestId		= ILV_CONFIG_UART;
	
	l_x_ILV_Buffer.m_puc_Buf	= NULL;
	l_x_ILV_SendBuffer.m_puc_Buf	= NULL;

	*o_puc_ILV_Status	= ILV_OK;

	l_x_IlvUartConfig.m_ul_BaudRate		= i_ul_BaudRate;
	l_x_IlvUartConfig.m_uc_ByteSize		= i_uc_ByteSize;
	l_x_IlvUartConfig.m_uc_StopBits		= i_uc_StopBits;
	l_x_IlvUartConfig.m_uc_Parity		= i_uc_Parity;
	l_x_IlvUartConfig.m_uc_FlowCtrl		= i_uc_FlowCtrl;
	l_x_IlvUartConfig.m_uc_Send			= i_uc_Send;
	l_x_IlvUartConfig.m_uc_Format		= i_uc_Format;
	
	l_x_V.m_ul_Size = sizeof(T_ILV_UART_CONFIG);
	l_x_V.m_puc_Buf	= (PUC)&l_x_IlvUartConfig;

	l_x_ILV_Buffer.m_ul_Size	= 10+ l_x_V.m_ul_Size;
	l_x_ILV_Buffer.m_puc_Buf	= malloc(l_x_ILV_Buffer.m_ul_Size);

	l_i_Ret = ILV_SetILV(
			   &l_x_ILV_Buffer, 
			   ID_COM1, 
			   &l_x_V,
			   &l_ul_ILVSize);

	if(!l_i_Ret)
	{
		l_x_V.m_ul_Size = l_ul_ILVSize;
		l_x_V.m_puc_Buf	= l_x_ILV_Buffer.m_puc_Buf;

		l_x_ILV_SendBuffer.m_ul_Size	= 10+ l_x_V.m_ul_Size;
		l_x_ILV_SendBuffer.m_puc_Buf = malloc(l_x_ILV_SendBuffer.m_ul_Size);

		l_i_Ret = ILV_SetILV(
				   &l_x_ILV_SendBuffer, 
				   l_us_RequestId, 
				   &l_x_V,
				   &l_ul_ILVSize);
	}

	if(!l_i_Ret)
	{
		l_i_Ret = MSO_SendReceive(i_h_Mso100Handle,
					l_us_RequestId,			//US	i_us_I,		// Identifier
					l_x_ILV_SendBuffer.m_puc_Buf,	//PUC	i_puc_Frame,	// data to send
					l_ul_ILVSize,			//UL	i_ul_len,	// number of bytes to write
					&l_puc_ILV_ReceiveBuffer,	//PUC	*o_puc_dataOut,	// response buffer
					&l_ul_ILVSize,			//PUL	o_pul_len,	// response length
					NULL,				//PUC *o_ppuc_Data,	// ILV data 
					o_puc_ILV_Status,		//PUC	o_puc_ILV_Status,
					NULL);				//PUL	o_pul_InternalErrorCode)
	}
	

	MSO_FrameRelease(i_h_Mso100Handle,&l_puc_ILV_ReceiveBuffer);

	if(l_x_ILV_Buffer.m_puc_Buf)
		free(l_x_ILV_Buffer.m_puc_Buf);

	if(l_x_ILV_SendBuffer.m_puc_Buf)
		free(l_x_ILV_SendBuffer.m_puc_Buf);

	return l_i_Ret;
}

