#ifndef _SP_USB_
#define _SP_USB_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

//*****************************************************************
/* F_SPUSB_ENUMDEVICES */
typedef I	(F_SPUSB_ENUMDEVICES)
( 
	PT_MSO_USB_DEVICE_PROPERTIES	*o_ppx_DeviceProperties,
	unsigned long			*o_pul_NbreDevices
);

typedef F_SPUSB_ENUMDEVICES *T_SPUSB_ENUMDEVICES;

F_SPUSB_ENUMDEVICES SpUsb_EnumDevices;


//*****************************************************************
/* F_SPUSB_RELEASEENUMDEVICES */
typedef I	(F_SPUSB_RELEASEENUMDEVICES)
( 
	PT_MSO_USB_DEVICE_PROPERTIES 	*o_ppx_DeviceProperties,
	unsigned long			i_ul_NbreDevices
);

typedef F_SPUSB_RELEASEENUMDEVICES *T_SPUSB_RELEASEENUMDEVICES;

F_SPUSB_RELEASEENUMDEVICES SpUsb_ReleaseEnumDevices;


//*****************************************************************
/* F_SPUSB_WAITFORREBOOT */
typedef I	(F_SPUSB_WAITFORREBOOT )
( 
	MORPHO_HANDLE	i_h_Handle,
	UL				i_ul_WriteTimeout,
	PUC				i_puc_Data,
	UL				i_ul_DataSize,
	UL				i_ul_WaitTimeout
);

typedef F_SPUSB_WAITFORREBOOT  *T_SPUSB_WAITFORREBOOT ;

F_SPUSB_WAITFORREBOOT SpUsb_WaitForReboot;


//////////////////////////////////////////////////////////
//		fonctions génériques
//		utilisées en callback
//////////////////////////////////////////////////////////
F_MORPHO_CALLBACK_COM_OPEN SpUsb_OpenEx;

//////////////////////////////////////////////////////////
F_MORPHO_CALLBACK_COM_SEND SpUsb_WriteFrame;

//////////////////////////////////////////////////////////
F_MORPHO_CALLBACK_COM_RECEIVE SpUsb_ReadFrame;

//////////////////////////////////////////////////////////
F_MORPHO_CALLBACK_COM_RECEIVE_FREE SpUsb_ReadFrameReleaseEx;

//////////////////////////////////////////////////////////
F_MORPHO_CALLBACK_COM_CLOSE SpUsb_Close;


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif

