#ifndef __LIBMSO_H
#define __LIBMSO_H

#include "libMSO_Def.h"
#include "libMSO_Struct.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define MSO_SERIAL_NUMBER_LEN           24      ///< Serial number max length

/* Connection Mode */
#define COM_USB                         "USB"   ///< USB Connection: See MSO_InitCom
#define COM_RS232                       "RS232" ///< RS232 Connection: See MSO_InitCom 

#define DEFAULT_COM_INTERFACE           COM_USB ///< Default Com interface: USB
#define DEFAULT_BAUD_RATE               115200  ///< RS232 default baudrate: 115200

/* Format for MSO_Get_Descriptor */
#define ID_FORMAT_TEXT                  47      ///< Text format for Get_Descriptor function
#define ID_FORMAT_BIN                   48      ///< Binary format for Get_Descriptor function

/* Security Flag: MSO_SECU_GetSerialNumber  */
///< Secure protocols are not supported on Linux
#define SECU_TUNNELING                  0x01    ///< If flag is set, the MorphoSmart uses the tunneling protocol
#define SECU_OFFERED_SECURITY           0x02    ///< If flag is set, the MorphoSmart uses the offered security protocol
#define SECU_PK_ONLY_SIGNED             0x04    ///< The MorphoSmart uses only templates with an X9.84 envelop and a signature
#define SECU_NEVER_EXPORT_SCORE         0x10    ///< If flag is set, the MorphoSmart never export its matching score


typedef I       (*T_pFuncILV_Buffer)(PVOID,I,PVOID); ///< Callback fonction: MSO_RegisterAsyncILV function


/*****************************************************************************/
/*****************************************************************************/
/* * * * * * * * * * * *  Connection managment function  * * * * * * * * * * */
/*****************************************************************************/
/*****************************************************************************/
/**
 * Initialize the connnection with the MorphoSmart. 
 * @param	i_ph_Mso100Handle: Handle to the device.
 * @param	i_str_Interface: Interface type: COM_USB or COM_RS232.
 * @param	i_str_comName: Serial device name: ex /dev/ttyS0. 
 *		Don't forget to change rigth if needed
 * @param	i_i_baudRate: RS232 connection baud rate: max 115200.
 *
 * @return        0   upon success,
 *              < 0   if an error occurs
 */
I MSO_InitCom
(
        MORPHO_HANDLE       *i_ph_Mso100Handle,
        PC                   i_str_Interface,
        PC                   i_str_comName,
        I                    i_i_baudRate
);


/*****************************************************************************/
/**
 * Close the communication 
 * @param	io_ph_Mso100Handle: Handle to the device. 
 * 		Set to NULL after closing the communication
 *
 * @return        0   upon success,
 *              < 0   if an error occurs
 */
I MSO_CloseCom
(
        MORPHO_HANDLE*      io_ph_Mso100Handle
);


/*****************************************************************************/
/**
 * Enumerate devices connected on USB bus.
 * This function allocate memory and fill the Device properties structure
 * @param	o_ppx_DeviceProperties: point to a tab of Device properties structure  
 * @param	o_pul_NbreDevices: point to the number of device connected 
 *
 * @return        0   upon success,
 *              < 0   if an error occurs
 */
I MSO_Usb_EnumDevices
(
        PT_MSO_USB_DEVICE_PROPERTIES    *o_ppx_DeviceProperties,
        PUL                              o_pul_NbreDevices
);

/*****************************************************************************/
/**
 * Release Device properties structure. 
 * this function release memory and and set o_ppx_DeviceProperties to NULL 
 * @param	o_ppx_DeviceProperties: point to a tab of Device properties structure  
 * @param	i_ul_NbreDevices: number of device structure to release 
 *
 * @return        0   upon success,
 *              < 0   if an error occurs
 * @see T_MSO_USB_DEVICE_PROPERTIES
 */
I MSO_Usb_ReleaseEnumDevices
(
        PT_MSO_USB_DEVICE_PROPERTIES    *o_ppx_DeviceProperties,
        UL                              i_ul_NbreDevices
);


/*****************************************************************************/
/**
 * Get information on device 
 * @param	i_h_Mso100Handle: Handle to the device. 
 * @param	i_uc_DescFormat: Format output: ID_FORMAT_TEXT or ID_FORMAT_BIN 
 * @param	o_puc_ILV_Status: Return status 
 * @param	o_pv_DescProduct: Product description 
 * @param	i_ul_SizeOfDescProduct: size of memory allocated for i_pv_DescProduct 
 * @param	o_pv_DescSensor: Sensor description 
 * @param	i_ul_SizeOfDescSensor: size of memory allocated for i_pv_DescSensor 
 * @param	o_pv_DescSoftware: Sofware description 
 * @param	i_ul_SizeOfDescSoftware: size of memory allocated for i_pv_DescSoftware  
 *
 * @return        0   upon success,
 *              < 0   if an error occurs
 */
I MSO_GetDescriptor
(
        MORPHO_HANDLE   i_h_Mso100Handle,
        UC              i_uc_DescFormat,
        PUC             o_puc_ILV_Status,
        VOID*           o_pv_DescProduct,
        UL              i_ul_SizeOfDescProduct,
        VOID*           o_pv_DescSensor,
        UL              i_ul_SizeOfDescSensor,
        VOID*           o_pv_DescSoftware,
        UL              i_ul_SizeOfDescSoftware	
);

/*****************************************************************************/
/*****************************************************************************/
/* * * * * * * * * * * *  Async Ilv management Function  * * * * * * * * * * */
/*****************************************************************************/
/*****************************************************************************/
/**
 * Register Callback function for asynchronous event. 
 * @param	i_h_Mso100Handle: Handle to the device 
 * @param	i_us_I: Mask of T_MORPHO_CALLBACK_COMMAND 
 * @param	i_p_Callback: Callback function. called when async events occurs 
 * @param	i_pv_context: Context to send to the Callback function 
 *
 * @return        0   upon success,
 *              < 0   if an error occurs
 * @see T_MORPHO_CALLBACK_COMMAND
 */
I MSO_RegisterAsyncILV
(
        MORPHO_HANDLE           i_h_Mso100Handle,
        I                       i_us_I,
        T_pFuncILV_Buffer       i_p_Callback,
        PVOID                   i_pv_context
);

/*****************************************************************************/
/**
 * UnRegister Callback function for asynchronous events. 
 * @param	i_h_Mso100Handle: Handle to the device 
 * @param	i_us_I: Mask of T_MORPHO_CALLBACK_COMMAND 
 *
 * @return        0   upon success,
 *              < 0   if an error occurs
 * @see T_MORPHO_CALLBACK_COMMAND
 */
I MSO_UnregisterAsyncILV
(
        MORPHO_HANDLE           i_h_Mso100Handle,
        I                       i_us_I
);

/*****************************************************************************/
/**
 * UnRegister Callback function for all asynchronous events. 
 * @param	i_h_Mso100Handle: Handle to the device 
 * @return        0   upon success,
 *              < 0   if an error occurs
 */
I MSO_UnregisterAllAsyncILV
(
        MORPHO_HANDLE           i_h_Mso100Handle
);

/*****************************************************************************/
/*****************************************************************************/
/* * * * * * * * * * * *  Base management Function * * * * * * * * * * * * * */
/*****************************************************************************/
/*****************************************************************************/
/**
 * Create a Database with various input paramater. 
 * @param	i_h_Mso100Handle: Handle to the device  
 * @param	i_uc_IndexDB: Base index, must be set to 0 
 * @param	i_us_UserMax: Max user in the DataBase 
 * @param	i_uc_NbFinger: Number of finger per User 
 * @param	i_uc_NormalizedPK_Type: If the value is different from 0, 
 *		it means that the database is normalized, otherwise templates are not 
 *		normalized. Normalization is reserved for specific usage. 
 * @param	i_uc_NbAddField: Number of additionnal field.
 * @param	i_px_AddField: Field Structure: Name, length, right  
 * @param	o_puc_ILV_Status: Return status 
 *
 * @return        0   upon success,
 *              < 0   if an error occurs
 * @see PT_FIELD
 */
I MSO_BioDB_CreateDb 
(
        MORPHO_HANDLE       i_h_Mso100Handle,
        UC                  i_uc_IndexDB,
        US                  i_us_UserMax,
        UC                  i_uc_NbFinger,
        UC                  i_uc_NormalizedPK_Type,
        UC                  i_uc_NbAddField,
        PT_FIELD            i_px_AddField,
        PUC                 o_puc_ILV_Status
);

/*****************************************************************************/
/**
 * Destroy database. 
 * The Database is totaly destroy. We must use the MSO_BioDB_CreateDb function 
 * to create a new base with new parameters
 * @param	i_h_Mso100Handle: Handle to the device 
 * @param	i_uc_IndexDB: Base index 
 * @param	o_puc_ILV_Status: Return status 
 * @param	o_pul_EmbeddedError: Return Embedded status ( 0 if no error) 
 * 
 * @return        0   upon success,
 *              < 0   if an error occurs
 */
I MSO_BioDB_DestroyDb 
(
        MORPHO_HANDLE       i_h_Mso100Handle,
        UC                  i_uc_IndexDB,
        PUC                 o_puc_ILV_Status,
        PUL                 o_pul_EmbeddedError
);

/*****************************************************************************/
/**
 * Delete User of index i_ul_IndexUser in the database. 
 * @param	i_h_Mso100Handle: Handle to the device 
 * @param	i_uc_IndexDB: Base index 
 * @param	i_ul_IndexUser: Index of User to delete 
 * @param	o_puc_ILV_Status: Return Status 
 * @param	o_pul_EmbeddedError: Embedded status ( 0 if no error) 
 *
 * @return        0   upon success,
 *              < 0   if an error occurs
 */
I MSO_BioDB_DeleteUser 
(
        MORPHO_HANDLE       i_h_Mso100Handle,
        UC                  i_uc_IndexDB,
        UL                  i_ul_IndexUser,
        PUC                 o_puc_ILV_Status,
        PUL                 o_pul_EmbeddedError
);

/*****************************************************************************/
/**
 * Get base configuration. 
 * @param	i_h_Mso100Handle: Handle to the device 
 * @param	i_uc_IndexDB: DataBase index 
 * @param	o_puc_FingerNb: Number of finger per User 
 * @param	o_pul_MaxRecord: Max Number of user in the DataBase 
 * @param	o_pul_CurrentRecord: Number of User recorded in the Database 
 * @param	io_pul_AddFieldNb: Number of additionnal field 
 * @param	o_px_AddField: Add Field Structure  
 * @param	o_puc_NormalizedPK_Type: Set to 0 for standard database
 * @param	o_puc_ILV_Status: Return Status 
 * @param	o_pul_EmbeddedError: Embedded status ( 0 if no error) 
 *
 * @return        0   upon success,
 *              < 0   if an error occurs
 * @see PT_FIELD
 */
I MSO_BioDB_GetBaseConfig 
(
        MORPHO_HANDLE       i_h_Mso100Handle,
        UC                  i_uc_IndexDB,
        PUC                 o_puc_FingerNb,
        PUL                 o_pul_MaxRecord,
        PUL                 o_pul_CurrentRecord,
        PUL                 io_pul_AddFieldNb,
        PT_FIELD            o_px_AddField,
        PUC                 o_puc_NormalizedPK_Type,
        PUC                 o_puc_ILV_Status,
        PUL                 o_pul_EmbeddedError
);


/*****************************************************************************/
/**
 * Erase Database.
 * The function erase all record in database but doesn't destroy the database structure 
 * @param	i_h_Mso100Handle: Handle to the device  
 * @param	i_uc_IndexDB: DataBase index 
 * @param	o_puc_ILV_Status: Return Status 
 * @param	o_pul_EmbeddedError: Return Embedded Status 
 *
 * @return        0   upon success,
 *              < 0   if an error occurs
 */
I MSO_BioDB_EraseDb 
(
        MORPHO_HANDLE       i_h_Mso100Handle,
        UC                  i_uc_IndexDB,
        PUC                 o_puc_ILV_Status,
        PUL                 o_pul_EmbeddedError
);


/*****************************************************************************/
/**
 * Get Data list in Database. 
 * @param	i_h_Mso100Handle: Handle to the device 
 * @param	i_uc_IndexDB: DataBase index 
 * @param	i_ul_UidData: Data type (0: UserID, 1: Add Field1, 2: Add Field2 ...) 
 * @param	io_pul_NbTransport: Number of Data 
 * @param	io_ax_TransportPublicField: Array of Data Structure 
 * @param	o_puc_ILV_Status: Return Status 
 *
 * @return        0   upon success,
 *              < 0   if an error occurs
 * io_pul_NbTransport must be set to number of structure io_ax_TransportPublicField allocated
 * Menmber m_puc_Data in io_ax_TransportPublicField must be also allocated
 * @see T_TRANSPORT_PUBLIC_LIST_FIELD
 */
I MSO_BioDB_GetPublicListData 
(
        MORPHO_HANDLE           i_h_Mso100Handle,
        UC                      i_uc_IndexDB,
        UL                      i_ul_UidData,
        PUL                     io_pul_NbTransport,
        PT_TRANSPORT_PUBLIC_LIST_FIELD  io_ax_TransportPublicField,
        PUC                     o_puc_ILV_Status
);

/*****************************************************************************/
/**
 * Add record  into Database. 
 * @param	i_h_Mso100Handle: Handle to the device 
 * @param	i_uc_IndexDB: DataBase index 
 * @param	i_uc_NbPk: Number of reference template 
 * @param	i_px_Pk: BUF_PK tab: idPk: ID_PKCOMP, ID_PKCOMP_NORM, ID_PKMAT, ID_PKMAT_NORM, ID_PKMOC, ID_PKBASE_INDEX  
 * @param	i_px_UserId: User ID 
 * @param	i_uc_NbAddField: Number of additionnal field 
 * @param	i_px_AddField:  AddField Tab 
 * @param	o_pul_IndexUser: Return index of user added in database 
 * @param	o_puc_ILV_Status: Return Status 
 * @param	o_puc_Base_Status: Return Base Status 
 * @param	i_b_NoCheckOnTemplate: Check on template flag 
 *
 * @return        0   upon success,
 *              < 0   if an error occurs
 * @see PT_BUF_PK, PT_BUF
 */
I MSO_BioDBAddBaseRecord
(
        MORPHO_HANDLE           i_h_Mso100Handle,
        UC                      i_uc_IndexDB,
        UC                      i_uc_NbPk,
        PT_BUF_PK               i_px_Pk,
        PT_BUF                  i_px_UserId,
        UC                      i_uc_NbAddField,
        PT_BUF                  i_px_AddField,
        PUL                     o_pul_IndexUser,
        PUC                     o_puc_ILV_Status,
        PUC                     o_puc_Base_Status,
        BOOL                    i_b_NoCheckOnTemplate
);




/*****************************************************************************/
/*****************************************************************************/
/* * * * * * * * * * * * * *  Biometric Function * * * * * * * * * * * * * * */
/*****************************************************************************/
/*****************************************************************************/
/**
 * Enrollment function.
 * @param	i_h_Mso100Handle: Handle to the device 
 * @param	i_uc_IndexDB: DataBase index 
 * @param	i_us_Timeout: Timeout for live finger acquisition, set to 0 for infinite timeout 
 * @param	i_uc_EnrollmentType: Set to 0 for 2 fingerprints acquisition per finger  
 * @param	i_uc_NbFinger: Number of finger 
 * @param	i_uc_SaveRecord: Set to 1 to store record in database  
 * @param	i_uc_NormalizedPK_Type: Set to 0 for standard database
 * @param	i_uc_NbAddField: Number of additionnal field 
 * @param	i_px_AddField: Add field tab 
 * @param	i_ul_AsynchronousEvent: Mask of Asynchronous events 
 * @param	io_px_ExportPk: allocate memory to export PK or set to NULL 
 * @param	io_px_ExportImage: allocate memory to export Image or Set to NULL  
 * @param	o_puc_EnrollStatus: Return Enrollment Status 
 * @param	o_pul_UserDBIndex: Return User database index 
 * @param	o_puc_ILV_Status: Return Status
 *
 * @return        0   upon success,
 *              < 0   if an error occurs
 * @see T_BUF, T_EXPORT_PK, T_EXPORT_IMAGE
 */
I MSO_Bio_Enroll 
(
        MORPHO_HANDLE           i_h_Mso100Handle,
        UC                      i_uc_IndexDB,
        US                      i_us_Timeout,
        UC                      i_uc_EnrollmentType,
        UC                      i_uc_NbFinger,
        UC                      i_uc_SaveRecord,
        UC                      i_uc_NormalizedPK_Type,
        UC                      i_uc_NbAddField,
        PT_BUF                  i_px_AddField,
        UL                      i_ul_AsynchronousEvent,
        PT_EXPORT_PK            io_px_ExportPk,
        PT_EXPORT_IMAGE         io_px_ExportImage,
        PUC                     o_puc_EnrollStatus,
        PUL                     o_pul_UserDBIndex,
        PUC                     o_puc_ILV_Status
);


/*****************************************************************************/
/**
 * Identify function.
 * @param	i_h_Mso100Handle: Handle to the device 
 * @param	i_uc_UidDB: DataBase index 
 * @param	i_us_Timeout: Timeout for live finger acquisition, set to 0 for infinite timeout 
 * @param	i_us_MatchingTreshold: Set value to 0 to 10, recommended 5 
 * @param	i_ul_AsynchronousEvent: Mask of Asynchronous events 
 * @param	o_puc_MatchingResult: Return matching result 
 * @param	o_pul_UserDBIndex: Return user base index 
 * @param	o_px_UserID: Return User ID 
 * @param	io_pul_AddFieldNb: Return number of additionnal field 
 * @param	o_px_AddFieldValue: Return add field tab 
 * @param	o_pul_score: Return score, Set o_pul_score to NULL if you don't want score   
 * @param	o_puc_ILV_Status: Return Status 
 *
 * @return        0   upon success,
 *              < 0   if an error occurs
 */
I MSO_Bio_Identify 
( 
	MORPHO_HANDLE       i_h_Mso100Handle,
        UC                  i_uc_UidDB,
        US                  i_us_Timeout,
        US                  i_us_MatchingTreshold,
        UL                  i_ul_AsynchronousEvent,
        PUC                 o_puc_MatchingResult,
        PUL                 o_pul_UserDBIndex,
        PT_BUF              o_px_UserID,
        PUL                 io_pul_AddFieldNb,
        PT_BUF              o_px_AddFieldValue,
        PUL                 o_pul_score,
        PUC                 o_puc_ILV_Status
);


/*****************************************************************************/
/**
 * Verify function.
 * @param	i_h_Mso100Handle: Handle to the device 
 * @param	i_us_Timeout: Timeout for live finger acquisition, set to 0 for infinite timeout 
 * @param	i_us_MatchingTreshold: Set value to 0 to 10, recommended 5 
 * @param	i_uc_NbFinger: number of finger 
 * @param	i_px_Pk: BUF_PK tab: idPk: ID_PKCOMP, ID_PKCOMP_NORM, ID_PKMAT, ID_PKMAT_NORM, ID_PKMOC, ID_PKBASE_INDEX 
 * @param	i_ul_AsynchronousEvent: Mask of Asynchronous event 
 * @param	o_puc_MatchingResult: Return matching result 
 * @param	o_pul_score: Return score, Set o_pul_score to NULL if you don't want score 
 * @param	o_puc_ILV_Status: Return Status
 * 
 * @return        0   upon success,
 *              < 0   if an error occurs
 * @see T_BUF_PK
 */
I MSO_Bio_Verify 
(
        MORPHO_HANDLE       i_h_Mso100Handle,
        US                  i_us_Timeout,
        US                  i_us_MatchingTreshold,
        UC                  i_uc_NbFinger,
        PT_BUF_PK           i_px_Pk,
        UL                  i_ul_AsynchronousEvent,
        PUC                 o_puc_MatchingResult,
        PUL                 o_pul_score,
        PUC                 o_puc_ILV_Status
);

/*****************************************************************************/
/**
 * Identify match function 
 * @param	i_h_Mso100Handle: Handle to the device 
 * @param	i_uc_UidDB: DataBase index 
 * @param	i_us_MatchingTreshold: Set value to 0 to 10, recommended 5 
 * @param	i_uc_NbPk: Number of PK 
 * @param	i_px_Pk: Buf_Pk tab 
 * @param	o_puc_MatchingResult: Return matching result 
 * @param	o_pul_UserDBIndex: Return User database index 
 * @param	o_px_UserID: Return User ID 
 * @param	o_pul_score: Return score, Set o_pul_score to NULL if you don't want score  
 * @param	o_puc_ILV_Status: Return Status 
 *
 * @return        0   upon success,
 *              < 0   if an error occurs
 * @see T_BUF, T_BUF_PK
 */
I MSO_Bio_IdentifyMatch 
(
        MORPHO_HANDLE           i_h_Mso100Handle,
        UC                      i_uc_UidDB,
        US                      i_us_MatchingTreshold,
        UC                      i_uc_NbPk,
        PT_BUF_PK               i_px_Pk,
        PUC                     o_puc_MatchingResult,
        PUL                     o_pul_UserDBIndex,
        PT_BUF                  o_px_UserID,
        PUL                     o_pul_score,
        PUC                     o_puc_ILV_Status
);

/*****************************************************************************/
/**
 * Verify Match function 
 * @param	i_h_Mso100Handle: Handle to the device 
 * @param	i_uc_NbPkSrc: Number of source Pk (Must be set to 1) 
 * @param	i_px_PkSrc:  Buf_Pk tab (type: ID_PKCOMP,ID_PKCOMP_NORM,ID_X984_BIO_TOKEN) 
 * @param	i_uc_NbPkRef: Number of Reference Pk 
 * @param	i_px_PkRef:  Reference Pk tab (type: ID_PKCOMP,ID_PKCOMP_NORM,ID_X984_BIO_TOKEN)
 * @param	i_us_MatchingTreshold: Set value to 0 to 10, recommended 5  
 * @param	o_puc_MatchingResult: Return matching result 
 * @param	o_puc_ListRefIndex: Return Index in i_px_PkRef tab 
 * @param	o_pul_score: Return score, Set o_pul_score to NULL if you don't want score  
 * @param	o_puc_ILV_Status: Return Status
 * 
 * @return        0   upon success,
 *              < 0   if an error occurs
 * @see T_BUF_PK
 */
I MSO_Bio_VerifyMatch 
(
        MORPHO_HANDLE           i_h_Mso100Handle,
        UC                      i_uc_NbPkSrc,
        PT_BUF_PK               i_px_PkSrc,
        UC                      i_uc_NbPkRef,
        PT_BUF_PK               i_px_PkRef,
        US                      i_us_MatchingTreshold,
        PUC                     o_puc_MatchingResult,
        PUC                     o_puc_ListRefIndex,
        PUL                     o_pul_score,
        PUC                     o_puc_ILV_Status
);



/*****************************************************************************/
/**
 * Cancel a live acquisition 
 * @param	i_h_Mso100Handle: Handle to the device 
 *
 * @return        0   upon success,
 *              < 0   if an error occurs
 */
I MSO_Cancel 
(
        MORPHO_HANDLE           i_h_Mso100Handle
);


/*****************************************************************************/
/*****************************************************************************/
/* * * * * * * * * * * SECU management Function  * * * * * * * * * * * * * * */
/*****************************************************************************/
/*****************************************************************************/
/**
 * Get serial number and Security config
 * @param	i_h_Mso100Handle: handle to the device 
 * @param	o_auc_SerialNumber: Return serial number 
 * @param	o_puc_SecuConfig: Return Secu config Mask 
 * @param	o_pus_SecuMaxFAR: Return Max Far, 
		Command with MathingTreshold<SecuMaxFAR are rejected 
 * @param	o_puc_ILV_Status: Return Status 
 *
 * @return        0   upon success,
 *              < 0   if an error occurs
 */
I MSO_SECU_GetSerialNumber
(
        MORPHO_HANDLE       i_h_Mso100Handle,
        UC                  o_auc_SerialNumber[MSO_SERIAL_NUMBER_LEN],
        PUC                 o_puc_SecuConfig,
        PUS                 o_pus_SecuMaxFAR,
        PUC                 o_puc_ILV_Status
);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
