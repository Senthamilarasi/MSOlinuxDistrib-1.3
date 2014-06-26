#ifndef __LIBMSO_Struct_H
#define __LIBMSO_Struct_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*****************************************************************************/
/* Device data struct */
/*****************************************************************************/
/**
* Device properties structure.
* For each enumerate device, a T_MSO_USB_DEVICE_PROPERTIES structure is filled
*/
typedef struct
{
        struct usb_device       *m_px_device;        ///< usb_device structure 
        PUC                     m_puc_SerialNumber;  ///< Device Serial number
        PUC                     m_puc_FriendlyName;  ///< Device Name (MSO300, CBM ...)
        UC                      m_uc_Index;          ///< Index Number in device list 
        PUC                     m_puc_DevicePath;    ///< Bus and device number: /proc/bus/usb/BBB/DDD 
}T_MSO_USB_DEVICE_PROPERTIES, *PT_MSO_USB_DEVICE_PROPERTIES;


/*****************************************************************************/
/* Buffer struct */
/*****************************************************************************/
/**
* Data Buffer structure.
*/
typedef struct
{
        UL                      m_ul_Size;  ///< Buffer size
        PUC                     m_puc_Buf;  ///< point to allocated buffer of size m_ul_Size
}T_BUF, *PT_BUF;


/*****************************************************************************/
/* Base data struct */
/*****************************************************************************/
#define MAX_FIELD_NAME_LEN       6  ///< Maximun length of field name
#define PUBLIC                   0  ///< Public field ID
#define PRIVATE                  1  ///< Private field ID

/**
* Bio data structure.
*/
typedef struct
{
        PUC                     m_puc_Data;     ///< Data buffer
        US                      m_us_SizeData;  ///< Size of Buffer
        UC                      m_uc_IdData;    ///< Data ID: ID_USER_ID or ID_USER_DATA
        UC                      m_uc_RFU;       ///< Reserved
}T_BIODB_DATA_TRANSPORT, *PT_BIODB_DATA_TRANSPORT;


/***************************************************************************/
/* Begin packed structure */
/***************************************************************************/
#pragma pack(1)
/**
* Field structure.
*/
typedef struct
{
        US                      m_us_FieldMaxSize;                      ///< Field Max size
        UC                      m_auc_FieldName[MAX_FIELD_NAME_LEN+1];  ///< Field Name
        UC                      m_uc_Right;                             ///< Field type: PUBLIC or PRIVATE
}T_FIELD, *PT_FIELD;

/**
* Public list field structure.
*/
typedef struct
{
        UL                      m_ul_UserIndex;    ///< User index in database
        UL                      m_ul_DataLenght;   ///< Data length
        PUC                     m_puc_Data;        ///< Buffer Data
}T_TRANSPORT_PUBLIC_LIST_FIELD, *PT_TRANSPORT_PUBLIC_LIST_FIELD;


/**
* ILV database structure. Use in MSO_BioDB_CreateDb.c
*/
typedef struct
{
        UC                      m_uc_IndexDB;    ///< Database Index
        UC                      m_uc_FlashType;  ///< Not used: set to 0
        US                      m_us_UserMax;    ///< Max number of user
        UC                      m_uc_PkMax;      ///< Max number of PK
}T_ILV_DB, *PT_ILV_DB;

/**
* Additionnal field structure. Use in MSO_BioDB_GetBaseConfig.c
*/
typedef struct
{
        US                      m_us_FieldMaxSize;                   ///< Field Max size
        UC                      m_auc_FieldName[MAX_FIELD_NAME_LEN]; ///< field name
}T_ILV_ADD_FIELD, *PT_ILV_ADD_FIELD;

/**
* Base configuration structure. Use in MSO_BioDB_GetBaseConfig.c
*/
typedef struct
{
        UC                      m_uc_RequestStatus;  ///< Request status
        UC                      m_uc_Finger;         ///< Number of finger per user
        UL                      m_ul_MaxRecord;      ///< Max number of database record
        UL                      m_ul_CurrentRecord;  ///< Current number of databse record
        UL                      m_ul_FreeRecord;     ///< Number of free database record
        UL                      m_ul_AddField;       ///< Number of additionnal field
}T_ILV_BASE_CONFIG, *PT_ILV_BASE_CONFIG;

#pragma pack()

/***************************************************************************/
/* End packed structure */
/***************************************************************************/



/*****************************************************************************/
/* Biometric data struct */
/*****************************************************************************/
#define ID_PKCOMP               2       //0x02  ///< PKCOMP (Recommanded minutiae)
#define ID_PKMAT                3       //0x03  ///< PKMAT (minutiae)
#define ID_PKMAT_NORM           53      //0x35  ///< PKMAT_NORM
#define ID_PKCOMP_NORM          55      //0x37  ///< PKCOMP_NORM
#define ID_PKBASE               58      //0x3A
#define ID_PKMOC                59      //0x3B

/**
* Public list field structure.
* @see MSO_Bio_Verify
*/
typedef struct
{
        T_BUF                   m_x_Buf;      ///< Pk Buffer
        UC                      m_uc_IdPk;    ///< PK type (ID_PKCOM Recommanded)
        UC                      m_uc_Index;   // Need for Iso Pk, see ID_ISO_PK_PARAM ILV
        UC                      m_uc_Pad[2];  //unused
}T_BUF_PK, *PT_BUF_PK;

/**
* Export Pk Structure.
* @see MSO_Bio_Enroll
*/
typedef struct
{
        PT_BUF                  io_px_BiometricData;  ///< Pk Buffer
        UC                      i_uc_ExportMinutiae;  ///< Export type: Set to 1 to export minutiae with default size
        UC                      pad[3];
}T_EXPORT_PK, *PT_EXPORT_PK;


/**
* Export Image Structure.
* @see MSO_Bio_Enroll
*/
typedef struct
{
        PT_BUF                  io_px_BufImage;         ///< Image Buffer
        UC                      i_uc_ExportImageType;   ///< Default: 0
        UC                      i_uc_CompressionType;   ///< Compression Type (ID_COMPRESSION_)
        UC						i_uc_CompressionParam;
		UC                      pad[1];
}T_EXPORT_IMAGE, *PT_EXPORT_IMAGE;


/**
* False acceptance reject value.
* From 0 to 10. Recommanded Value is 5 
*/
typedef enum {
        MORPHO_FAR_0 = 0,
        MORPHO_FAR_1,
        MORPHO_FAR_2,
        MORPHO_FAR_3,
        MORPHO_FAR_4,
        MORPHO_FAR_5,
        MORPHO_FAR_6,
        MORPHO_FAR_7,
        MORPHO_FAR_8,
        MORPHO_FAR_9,
        MORPHO_FAR_10
} T_MORPHO_FAR,*PT_MORPHO_FAR;



/**
* Image header structure.
*/
typedef struct
{
        UC                       m_uc_HeaderRevision;  ///< Header Revision number
        UC                       m_uc_HeaderSize;      ///< Header size
        US                       m_us_NbRow;           ///< Row number
        US                       m_us_NbCol;           ///< Column number
        US                       m_us_ResY;            ///< Y axis resolution
        US                       m_us_ResX;            ///< X axis resolution
        UC                       m_uc_CompressionType; ///< Compression type
        UC                       m_uc_NbBitsPerPixel;  ///< Bits per pixel (8)
}T_MORPHO_IMAGE_HEADER;


/**
* Asynchronous event flag. 
* We must register a callback function with wanted Asynchronous event
* @see MSO_RegisterAsyncILV
*/
typedef enum {
        MORPHO_CALLBACK_COMMAND_CMD                     = 1,     ///< 1<<0 Asynchronous command
        MORPHO_CALLBACK_IMAGE_CMD                       = 2,     ///< 1<<1 Asynchronous image
        MORPHO_CALLBACK_ENROLLMENT_CMD                  = 4,     ///< 1<<2 Asynchronous enrollment command
        MORPHO_CALLBACK_LAST_IMAGE_FULL_RES_CMD         = 8,     ///< 1<<3 Asynchronous image
        MORPHO_CALLBACK_RESERVED1                       = 16,    ///< 1<<4
        MORPHO_CALLBACK_RESERVED2                       = 32,    ///< 1<<5
        MORPHO_CALLBACK_CODEQUALITY                     = 64,    ///< 1<<6 Asynchronous code quality
        MORPHO_CALLBACK_DETECTQUALITY                   = 128,   ///< 1<<7 Asynchronous detect quality
        MORPHO_CALLBACK_RESERVED3                       = 256    ///< 1<<8
} T_MORPHO_CALLBACK_COMMAND;


/**
* Command status. 
* Status returned when MORPHO_CALLBACK_COMMAND_CMD event occurs
* @see MSO_RegisterAsyncILV
*/
typedef enum {
        MORPHO_MOVE_NO_FINGER,      ///< No finger detected
        MORPHO_MOVE_FINGER_UP,      ///< You need to move up your finger
        MORPHO_MOVE_FINGER_DOWN,    ///< You need to move down your finger
        MORPHO_MOVE_FINGER_LEFT,    ///< You need to move left your finger
        MORPHO_MOVE_FINGER_RIGHT,   ///< You need to move rigth your finger
        MORPHO_PRESS_FINGER_HARDER, ///< You need to press harder wih your finger
        MORPHO_LATENT,
        MORPHO_REMOVE_FINGER,       ///< You need to remove your finger
        MORPHO_FINGER_OK            ///< Finger acquisition is OK
} T_MORPHO_COMMAND_STATUS;

/**
* Image Commpression algorythme.
*/
typedef enum {
        MORPHO_NO_COMPRESS,     ///< No compression
        MORPHO_COMPRESS_V1      ///< Huffman compression
} T_MORPHO_COMPRESS_ALGO;

/**
* Enrollment command status. 
* Status returned when MORPHO_CALLBACK_ENROLLMENT_CMD event occurs
*/
typedef struct {
        UC                              m_uc_nbFinger;      ///< Current number of finger acquisition
        UC                              m_uc_nbFingerTotal; ///< Total number of finger needed for one finger 
        UC                              m_uc_nbCapture;     ///< Current number of finger captured
        UC                              m_uc_nbCaptureTotal;///< Total number of finger to capture
} T_MORPHO_CALLBACK_ENROLLMENT_STATUS;

/**
* Image status.
* Image returned when MORPHO_CALLBACK_IMAGE_CMD event occurs
*/
typedef struct {
        T_MORPHO_IMAGE_HEADER   	m_x_ImageHeader;  ///< Image header
        PUC                             m_puc_Image;  ///< Image buffer
} T_MORPHO_CALLBACK_IMAGE_STATUS, T_MORPHO_IMAGE;


/**
* Callback function prototype 
*/
typedef I  (*T_MORPHO_CALLBACK_FUNCTION)(
                          PVOID                           i_pv_context, ///< Context send to callback function
                          T_MORPHO_CALLBACK_COMMAND       i_i_command,  ///< Asynchronous command type
                          PVOID                           i_pv_param);  ///< Asynchronous data


/**
* Pk Type 
* Recommanded Pk type is MORPHO_PK_COMP
*/
typedef enum {
                MORPHO_PK_COMP,
                MORPHO_PK_MAT_NORM,
                MORPHO_PK_COMP_NORM,
                MORPHO_PK_MAT,
                MORPHO_PK_ANSI_378,
                MORPHO_PK_MINEX_A,
                MORPHO_PK_ISO_FMR,
                MORPHO_PK_ISO_FMC_NS,
                MORPHO_PK_ISO_FMC_CS,
                MORPHO_PK_ILO_FMR
} T_MORPHO_TYPE_TEMPLATE;

/**
* Template envelop type 
* @see MSO_Bio_EnrollToken
*/
typedef enum {
                MORPHO_RAW_TEMPLATE,         ///< For raw Template
                MORPHO_X984_SIGNED_TEMPLATE  ///< For signed template 
}T_MORPHO_TEMPLATE_ENVELOP;


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
