#include <stdio.h>
#include <string.h>
#include <usb.h>

#include "libMSO.h"
#include "MSO100.h"

#include "Usb_Drv.h"
#include "Usb_Drv_Interface.h"


/*****************************************************************************/
/*****************************************************************************/
I MSO_Usb_EnumDevices
( 
    PT_MSO_USB_DEVICE_PROPERTIES    *o_ppx_DeviceProperties,
    unsigned long                   *o_pul_NbreDevices
)
{
    return SpUsb_EnumDevices ( o_ppx_DeviceProperties, o_pul_NbreDevices );
}


/*****************************************************************************/
/*****************************************************************************/
I MSO_Usb_ReleaseEnumDevices
( 
    PT_MSO_USB_DEVICE_PROPERTIES	*o_ppx_DeviceProperties,
    unsigned long			i_ul_NbreDevices
)
{
    return SpUsb_ReleaseEnumDevices ( o_ppx_DeviceProperties, i_ul_NbreDevices );
}

 
/*****************************************************************************/
/*****************************************************************************/
I MSO_WaitForReboot 
(
    MORPHO_HANDLE i_h_Mso100Handle,
    UL            i_ul_WriteTimeout,
    PUC           i_puc_Data,
    UL            i_ul_DataSize,
    UL            i_ul_WaitTimeOut
)
{
    void **l_px_usb_dev_handle;

    if(i_h_Mso100Handle==NULL)
        return MSO_BAD_PARAMETER;

    l_px_usb_dev_handle = &((PT_MSO100_STRUCT)i_h_Mso100Handle)->m_h_Handle;

    return SpUsb_WaitForReboot( (struct usb_dev_handle **) l_px_usb_dev_handle,
				i_ul_WriteTimeout,
				i_puc_Data,
				i_ul_DataSize,
				i_ul_WaitTimeOut);
    
}

/*****************************************************************************/
/*****************************************************************************/
