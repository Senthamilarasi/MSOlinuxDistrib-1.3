#ifndef __USB_DRV_H
#define __USB_DRV_H

#define MAX_USB_DEVICE              10
#define MAX_USB_FRAME_SIZE          1000*1000

#define SAGEM_VENDOR_ID             0x079b

#define SOFTWAREID_MSO100           "MSO100"
#define SOFTWAREID_MSO300           "MSO300"
#define SOFTWAREID_MSO350           "MSO350"
#define SOFTWAREID_MSOTEST          "MSOXXX"
#define SOFTWAREID_CBM              "CBM"
#define SOFTWAREID_MSO1310          "MSO1310"

#define PRODUCTID_MSO100            0x0023
#define PRODUCTID_MSO300            0x0024
#define PRODUCTID_MSO350            0x0026
#define PRODUCTID_MSOTEST           0x0025
#define PRODUCTID_CBM               0x0047
#define PRODUCTID_1350              0x0052

#define SPUSB_SYNC_FRAME            "SYNC"
#define SPUSB_SYNC_FRAME_LEN        4
#define SPUSB_END_FRAME             "EN"
#define SPUSB_END_FRAME_LEN         2

#define SPUSB_TIMEOUT_INFINITE      0xFFFFFFFF

#endif
