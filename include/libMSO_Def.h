#ifndef __LIBMSO_Def_H
#define __LIBMSO_Def_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*!
*  Simple type
*/
#define VOID                    void
#define UC                      unsigned char
#define C                       char
#define US                      unsigned short
#define S                       short
#define UL                      unsigned long
#define L                       long
#define I                       int
#define UI                      unsigned int
#define PUI                     unsigned int*
#define PVOID                   void*
#define PUC                     unsigned char*
#define PC                      char*
#define PUS                     unsigned short*
#define PS                      short*
#define PUL                     unsigned long*
#define PL                      long*
#define PI                      int*

#define BOOLEAN                 UC
#define BOOL                    UC
#define B                       UC
#define DWORD                   unsigned long
#define HANDLE                  void *

typedef void*                   MORPHO_HANDLE;	///< Handle to a device

#define RETURN_NO_ERROR        0    		///< No error

/*!
*  Status code return by function: o_puc_ILV_Status.
* 
*/
#define ILV_OK                          0x00    ///< Successful result
#define ILVERR_ERROR                    0xFF    ///< An error occurred
#define ILVERR_BADPARAMETER             0xFE    ///< Input parameters are not valid
#define ILVERR_INVALID_MINUTIAE         0xFD    ///< The minutiae is not valid
#define ILVERR_INVALID_USER_ID          0xFC    ///< The record identifier does not exist in the database
#define ILVERR_INVALID_USER_DATA        0xFB    ///< The user data are not valid
#define ILVERR_TIMEOUT                  0xFA    ///< No response after defined time.
#define ILVERR_INVALID_ID_PROTOCOL      0xF9    ///< The protocole used is not valid
#define ILVERR_ALREADY_ENROLLED         0xF8    ///< The person is already in the base
#define ILVERR_BASE_NOT_FOUND           0xF7    ///< The specified base does not exist
#define ILVERR_BASE_ALREADY_EXISTS      0xF6    ///< The specified base already exist
#define ILVERR_BIO_IN_PROGRESS          0xF5    ///< Command received during biometric processing
#define ILVERR_CMD_INPROGRESS           0xF4    ///< Command received while another command is running
#define ILVERR_FLASH_INVALID            0xF3    ///< Flash type invalid
#define ILVERR_NO_SPACE_LEFT            0xF2    ///< Not Enough memory for the creation of a database
#define ILVERR_FIELD_NOT_FOUND          0xE9    ///< Field does not exist.
#define ILVERR_FIELD_INVALID            0xE8    ///< Field size or field name is invalid.
#define ILVERR_SECURITY_MODE            0xE7    ///< The request is not compatible with security mode.
#define ILVERR_USER_NOT_FOUND           0xE6    ///< The searched user is not found.
#define ILVERR_CMDE_ABORTED             0xE5    ///< Commanded has been aborted by the user.
#define ILVERR_SAME_FINGER              0xE4    ///< There are two templates of the same finger
#define ILVERR_NO_HIT                   0xE3    ///< Presented finger does not match
#define ILVERR_FFD                      0xDB    ///< False finger detected
#define ILVERR_MOIST_FINGER             0xDA    ///< Too moist finger detected 

#define ILVERR_NOT_IMPLEMENTED          0x9D    ///< The request is not yet implemented.

/*!
* Status Codes definition : 
*/
#define ILVSTS_OK                       0       ///< Successful
#define ILVSTS_HIT                      1       ///< Authentication or Identification succeed
#define ILVSTS_NO_HIT                   2       ///< Authentication or Identification failed
#define ILVSTS_LATENT                   3       ///< Security Protection Triggered
#define ILVSTS_DB_FULL                  4       ///< The database is full
#define ILVSTS_DB_EMPTY                 5       ///< The database is empty
#define ILVSTS_BAD_QUALITY              6       ///< Bad finger and/or enroll quality
#define ILVSTS_DB_OK                    7       ///< The database is right
#define ILVSTS_ACTIVATED                8       ///< The MorphoModule is activated
#define ILVSTS_NOTACTIVATED             9       ///< The MorphoModule is not activated
#define ILVSTS_DB_KO                    10      ///< The flash can not be accessed

#define ILVSTS_FFD			0x22    ///< False finger detected
#define ILVSTS_MOIST_FINGER		0x23    ///< Too moist finger detected

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

