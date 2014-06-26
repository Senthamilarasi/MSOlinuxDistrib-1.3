#ifndef _SP_RS232
#define _SP_RS232

/*******************************************************************************
-- definition
*******************************************************************************/
#define MAX_DATA_SIZE      1024
#define ACK_TIMEOUT        1000

#define	PARTIAL_FRAME      0
#define COMPLETE_FRAME     1

#define SPRS232_RECEIVE_MAX    400 * 1024  //400ko
#define SPRS232_RECEIVE_ALLOC  SPRS232_RECEIVE_MAX + 20

#ifdef __cplusplus
extern "C"
{
#endif

#pragma pack(1)
typedef struct
{
        UL                      m_ul_BaudRate;
        UC                      m_uc_ByteSize;
        UC                      m_uc_StopBits;
        UC                      m_uc_Parity;
        UC                      m_uc_FlowCtrl;
        UC                      m_uc_Send;
        UC                      m_uc_Format;
}T_ILV_UART_CONFIG, *PT_ILV_UART_CONFIG;
#pragma pack()

//////////////////////////////////////////////////////////
//		fonctions g��iques
//		utilis�s en callback
//////////////////////////////////////////////////////////
F_MORPHO_CALLBACK_COM_OPEN SPRS232_OpenEx;

F_MORPHO_CALLBACK_COM_SEND SPRS232_SendEx;

F_MORPHO_CALLBACK_COM_RECEIVE SPRS232_ReceiveEx;

F_MORPHO_CALLBACK_COM_RECEIVE_FREE SPRS232_ReceiveFreeEx;

F_MORPHO_CALLBACK_COM_CLOSE SPRS232_Close;

/*******************************************************************************/
I SPRS232_SetBaudRate
(
	MORPHO_HANDLE	i_h_Mso100Handle,
	DWORD	i_dw_BaudRate
);

I SPRS232_Break
(
	MORPHO_HANDLE	i_h_Mso100Handle,
	UL i_ul_Time
);

BOOL SPRS232_IsInitialize
(
	MORPHO_HANDLE	i_h_Mso100Handle
);

I SPRS232_Write
(
	MORPHO_HANDLE	i_h_Mso100Handle,
	UC *i_puc_Buf,
	UL i_ul_Size,
	UL *o_pul_BytesWritten
);

I SPRS232_Read
(
	MORPHO_HANDLE	i_h_Mso100Handle,
	PUC	i_puc_Buf,
	UL	i_ul_Size,
	UL	i_ul_ReadTotalTimeoutConstant
);

I SPRS232_GetConfig 
(	
	MORPHO_HANDLE	i_h_Mso100Handle
);

////////////////////////////////////////////////////////////
I SPRS232_Initialize
(
	MORPHO_HANDLE	*o_ph_Mso100Handle,
	C * i_strName,
	UL i_ul_BaudRate
);

I SPRS232_Send
(
	MORPHO_HANDLE	i_h_Mso100Handle,
	UC * i_pData,
	UL i_nDataSize
);

I SPRS232_Receive
(
	MORPHO_HANDLE	i_h_Mso100Handle,
	UC * i_puc_Buffer, 
	UL i_ul_BufferSize, 
	UL *o_pul_ILVSize,
	DWORD i_dw_ReadTotalTimeoutConstant
);

I SPRS232_ReceiveOneFrame
(
	MORPHO_HANDLE	i_h_Mso100Handle,
	UC * i_puc_Buffer, 
	UL i_ul_BufferSize, 
	UL *o_pul_ILVSize,
	DWORD i_dw_ReadTotalTimeoutConstant,
	UC*	o_puc_WholeFrameReceived
);


#ifdef __cplusplus
}
#endif

#endif
