#ifndef __DRV_RS232_H
#define __DRV_RS232_H

#include "MSO100.h"

#ifdef __cplusplus
extern "C"
{
#endif


I RS232_Initialize
(
    C *		i_pc_strName,
    UC		i_uc_XonValue, 
    UC		i_uc_XoffValue,
    UL		i_dw_BaudRate
);

I RS232_SetBaudRate
(
    UC		i_uc_XonValue, 
    UC		i_uc_XoffValue,
    UL		i_dw_BaudRate
);

I RS232_Close
(
    void
);

I RS232_Write
(
    UC 		*i_puc_Buf,
    UL 		i_ul_Size,
    UL 		*o_pul_BytesWritten
);


I RS232_Read
(
    UC 		*i_puc_Buf,
    UL 		i_ul_Size,
    UL		i_dw_ReadTotalTimeoutConstant
);

I RS232_Break
(
    UL 		i_ul_Time
);

I RS232_IsInitialize
(
    void
);

UL RS232_ReturnHandlePort
( 
    void
);

I RS232_GetConfig
(
    void
);

I RS232_Clear
(
    void
);

#ifdef __cplusplus
}
#endif

#endif
