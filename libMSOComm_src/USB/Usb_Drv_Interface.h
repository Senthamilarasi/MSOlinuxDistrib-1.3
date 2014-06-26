#ifndef __USB_DRV_INTERFACE_H
#define __USB_DRV_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*****************************************************************************/
/*****************************************************************************/
int SpUsb_OpenEx
(
    struct usb_dev_handle      **o_h_Mso100Handle,
    PC                         i_pc_String,
    UL                         i_ul_TimeOut,
    PVOID                      io_pv_Param
);

/*****************************************************************************/
/*****************************************************************************/
int SpUsb_Close
(
    struct usb_dev_handle      **i_h_Mso100Handle
);

/*****************************************************************************/
/*****************************************************************************/
int SpUsb_WriteFrame
(
    struct usb_dev_handle      *i_h_Mso100Handle,
    UL                         i_ul_Timeout,
    PUC                        i_puc_Data,
    UL                         i_ul_DataSize
);

/*****************************************************************************/
/*****************************************************************************/
int SpUsb_ReadFrame
(
    struct usb_dev_handle      *i_h_Mso100Handle,
    UL                         i_ul_Timeout,
    PUC*                       io_ppuc_DataReceive,
    PUL                        o_pul_DataSize
);

/*****************************************************************************/
/*****************************************************************************/
int SpUsb_EnumDevices
(
    PT_MSO_USB_DEVICE_PROPERTIES    *o_ppx_DeviceProperties,
    PUL                 	    o_pul_NbDevices
);

/*****************************************************************************/
/*****************************************************************************/
int SpUsb_ReadFrameReleaseEx
(
    struct usb_dev_handle      *i_h_Mso100Handle,
    PUC*                       io_ppuc_Data
);

/*****************************************************************************/
/*****************************************************************************/
int SpUsb_ReleaseEnumDevices
( 
    PT_MSO_USB_DEVICE_PROPERTIES    *o_ppx_DeviceProperties,
    UL                		    i_ul_NbreDevices
);

/*****************************************************************************/
/*****************************************************************************/
int SpUsb_WaitForReboot
( 
    struct usb_dev_handle      **i_ph_Mso100Handle,
    UL                         i_ul_WriteTimeout,
    PUC                        i_puc_Data,
    UL                         i_ul_DataSize,
    UL                         i_ul_WaitTimeOut
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

