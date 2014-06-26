#ifndef __MSO100_H
#define __MSO100_H

#include "errors.h"

#define NB_RECORDED_ILV              30
#define ILV_SENDBUFFER_SIZE          8192

#define READ_TIMEOUT                 7000
#define WRITE_TIMEOUT                7000
#define IDENT_TIMEOUT                10000  //wait timeout for identification process
#define COMERR_TIMEOUT_RECEIVE      -10000  // The timeout error of the callback receive function.
#define TEMPO_RECEIVE                100    // 100 ms

#define min(a,b)     ((a<b)?a:b)
#define INFINITE     0xFFFFFFFF     // Infinite timeout

#define Sleep(a)    usleep(a*1000)  // Sleep(ms)    usleep(micro sec)
#define strnicmp    strncasecmp
#define stricmp     strcasecmp

#ifndef TRUE
#       define TRUE     (1==1)
#endif

#ifndef FALSE
#       define FALSE    (0==1)
#endif

#ifndef NULL
#       define NULL     0
#endif


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*****************************************************************************/
/** CALLBACK COM ***/
/*****************************************************************************/
/* T_MORPHO_CALLBACK_COM_OPEN */
typedef I       (F_MORPHO_CALLBACK_COM_OPEN)(
                                        MORPHO_HANDLE   *o_ph_MsoHandle,
                                        PC              i_pc_String,
                                        UL              i_ul_Param,
                                        PVOID           io_pv_Param     );

typedef F_MORPHO_CALLBACK_COM_OPEN *T_MORPHO_CALLBACK_COM_OPEN;

/* T_MORPHO_CALLBACK_COM_SEND */
typedef I       (F_MORPHO_CALLBACK_COM_SEND)(
                                        MORPHO_HANDLE   i_h_MsoHandle,
                                        UL              i_ul_Timeout,
                                        PUC             i_puc_Data,
                                        UL              i_ul_DataSize);

typedef F_MORPHO_CALLBACK_COM_SEND *T_MORPHO_CALLBACK_COM_SEND;

/* T_MORPHO_CALLBACK_COM_RECEIVE */
typedef I       (F_MORPHO_CALLBACK_COM_RECEIVE)(
                                        MORPHO_HANDLE   i_h_MsoHandle,
                                        UL              i_ul_Timeout,
                                        PUC*            io_ppuc_Data,
                                        PUL             o_pul_DataSize);

typedef F_MORPHO_CALLBACK_COM_RECEIVE *T_MORPHO_CALLBACK_COM_RECEIVE;

/* T_MORPHO_CALLBACK_COM_RECEIVE_FREE */
typedef I       (F_MORPHO_CALLBACK_COM_RECEIVE_FREE)(
                                        MORPHO_HANDLE   i_h_MsoHandle,
                                        PUC*            io_ppuc_Data);

typedef F_MORPHO_CALLBACK_COM_RECEIVE_FREE *T_MORPHO_CALLBACK_COM_RECEIVE_FREE;

/* T_MORPHO_CALLBACK_COM_CLOSE */
typedef I       (F_MORPHO_CALLBACK_COM_CLOSE)(
                                        MORPHO_HANDLE   *i_ph_MsoHandle);

typedef F_MORPHO_CALLBACK_COM_CLOSE *T_MORPHO_CALLBACK_COM_CLOSE;



/*****************************************************************************/
typedef struct
{
        US                      m_us_Ident;             /* Identifier that will trigger callback function */
        T_pFuncILV_Buffer       m_x_Callback;           /* callback function */
        PVOID                   m_pv_Context;
}T_ILV_BUFFER;

typedef struct
{
        B                                       m_b_Initialised;
        C                                       m_str_Interface[20];

        MORPHO_HANDLE                           m_h_Handle;

        I                                       g_i_Stop                /* = 1*/;
        I                                       g_i_EmbeddedError;

        T_MORPHO_CALLBACK_COM_OPEN              g_pf_CallbackComOpen    /* = NULL*/;
        T_MORPHO_CALLBACK_COM_SEND              g_pf_CallbackComSend    /* = NULL*/;
        T_MORPHO_CALLBACK_COM_RECEIVE           g_pf_CallbackComReceive /* = NULL*/;
        T_MORPHO_CALLBACK_COM_RECEIVE_FREE      g_pf_CallbackComReceiveFree/* = NULL*/;
        T_MORPHO_CALLBACK_COM_CLOSE             g_pf_CallbackComClose   /* = NULL*/;

        I                                       g_i_nb_RecordedILV;
        T_ILV_BUFFER                            g_px_IlvTable[NB_RECORDED_ILV];

} T_MSO100_STRUCT, *PT_MSO100_STRUCT;

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
I MSO_NegociateBaudRate
(
	MORPHO_HANDLE	*i_h_Mso100Handle,
	PC		i_pc_String,
	UL		i_ul_Param,
	PVOID		io_pv_Param
);

/////////////////////////////////////////////////////////////////////
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
);

/////////////////////////////////////////////////////////////////////
I MSO_ComOpen
(
	MORPHO_HANDLE				*o_ph_Mso100Handle,
	T_MORPHO_CALLBACK_COM_OPEN		i_pf_CallbackComOpen,		
	T_MORPHO_CALLBACK_COM_SEND		i_pf_CallbackComSend,	
	T_MORPHO_CALLBACK_COM_RECEIVE		i_pf_CallbackComReceive,	
	T_MORPHO_CALLBACK_COM_RECEIVE_FREE	i_pf_CallbackComReceiveFree,
	T_MORPHO_CALLBACK_COM_CLOSE		i_pf_CallbackComClose,
	PC					i_pc_String,
	UL					i_ul_Param,
	PVOID					io_pv_param
);

/////////////////////////////////////////////////////////////////////
I MSO_ComSend 
( 
	MORPHO_HANDLE	i_h_Mso100Handle,
	UL		i_ul_Timeout,
	PUC		i_puc_Data,
	UL		i_ul_DataSize
);

/////////////////////////////////////////////////////////////////////
I MSO_ComReceive 
( 
	MORPHO_HANDLE	i_h_Mso100Handle,
	UL		i_ul_Timeout,
	PUC*		io_ppuc_Data,
	PUL		o_pul_DataSize
);

/////////////////////////////////////////////////////////////////////
I
MSO_ComReceiveFree 
( 
	MORPHO_HANDLE	i_h_Mso100Handle,
	PUC*		io_ppuc_Data
);

/////////////////////////////////////////////////////////////////////
I MSO_CloseCom
(
	MORPHO_HANDLE*	io_h_Mso100Handle
);

/////////////////////////////////////////////////////////////////////
I MSO_SendReceive
(
	MORPHO_HANDLE	i_h_Mso100Handle,
	US		i_us_I,		// Identifier
	PUC		i_puc_Frame,	// data to send
	UL		i_ul_len,	// number of bytes to write
	PUC		*o_puc_dataOut,	// response buffer
	PUL		o_pul_len,	// response length
	PUC		*o_ppuc_Data,	// ILV data 
	PUC		o_puc_ILV_Status,
	PI		o_pi_EmbeddedError
);

/////////////////////////////////////////////////////////////////////
I MSO_SendReceiveTimeout
(
	MORPHO_HANDLE	i_h_Mso100Handle,
	UL		i_ul_TransmitTimeout,	// reception timeout, only for USB (ms)
	UL		i_ul_ReceiveTimeout,	// reception timeout (ms)
	US		i_us_I,			// Identifier
	PUC		i_puc_Frame,		// data to send
	UL		i_ul_len,		// number of bytes to write
	PUC		*o_ppuc_dataOut,	// response buffer
	PUL		o_pul_len,		// response length
	PUC		*o_ppuc_Data,		// ILV data 
	PUC		o_puc_ILV_Status,
	PI		o_pi_EmbeddedError
);

/////////////////////////////////////////////////////////////////////
void MSO_FrameRelease
(
	MORPHO_HANDLE	i_h_Mso100Handle,
	PUC 		*l_puc_Buffer
);


/////////////////////////////////////////////////////////////////////
I MSO_GetLastEmbeddedError
(
	MORPHO_HANDLE	i_h_Mso100Handle
);

/////////////////////////////////////////////////////////////////////
I MSO_RunAsyncILV
(	
	MORPHO_HANDLE	i_h_Mso100Handle,
	US		i_us_Ident,
	PUC		i_puc_dataOut
);

/////////////////////////////////////////////////////////////////////
I MSO_Break
(
	MORPHO_HANDLE		i_h_Mso100Handle,
	UL			i_ul_Time
);


/////////////////////////////////////////////////////////////////////
I MSO_SetPk
(
        UC              i_uc_NbPk,
        PT_BUF_PK       i_px_Pk,
        PT_BUF          io_px_MainIlv,
        PUL             io_pul_MainIlvSize
);

/////////////////////////////////////////////////////////////////////
I MSO_GetPk
(
        UC              i_uc_NbFinger,
        PUC             i_puc_ReceivedIlv,
        PUL             i_pul_ReceivedIlvSize,
        PT_EXPORT_PK    o_px_ExportPk
);


/////////////////////////////////////////////////////////////////////
I MSO_SetTkb
(
        PT_BUF_PK       i_px_X984,
        UC              i_uc_PkIndex,
        PT_BUF          io_px_MainIlv,
        PUL             io_pul_MainIlvSize
);

/////////////////////////////////////////////////////////////////////
I MSO_SetIsoPk
(
	PT_BUF_PK 	i_px_Pk, 
	PT_BUF 		io_px_MainIlv, 
	PUL     	io_pul_MainIlvSize
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

