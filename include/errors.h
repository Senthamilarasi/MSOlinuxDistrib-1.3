#ifndef __ERRORS_H
#define __ERRORS_H

/*!
*  function return code: embedded errors
*
*/
#define BIO_ERROR_INVALID_PK_TYPE                       -480    ///< 0xE20: Invalid Pk type
#define BIO_ERROR_AUTHENT_FAILED                        -481    ///< 0xE1F: Authentification failed
#define BIO_ERROR_CODE_FAILED                           -482    ///< 0xE1E
#define BIO_RET_CONSO_FAILED                            -483    ///< 0xE1D
#define BIO_ERROR_UPDATE_LATENT_FAILED                  -484    ///< 0xE1C
#define BIO_ERROR_LATENT_DETECT_FAILED                  -485    ///< 0xE1B
#define BIO_ERROR_NOT_OPEN                              -486    ///< 0xE1A
#define BIO_ERROR_CONVERT_PKCOMP_TO_PKBASE_FAILED       -487    ///< 0xE19
#define BIO_ERROR_CONVERT_PKBASE_TO_PKCOMP_FAILED       -488    ///< 0xE18
#define BIO_ERROR_BAD_NUMBER_OF_PK                      -489    ///< 0xE17
#define BIO_ERROR_BAD_PK_NUMBER                         -490    ///< 0xE16
#define BIO_RET_LATENT_DETECTED                         -491    ///< 0xE15
#define BIO_ERROR_IDENT_FAILED                          -492    ///< 0xE14
#define BIO_RET_DB_FULL                                 -493    ///< 0xE13 Database is full
#define BIO_RET_DOUBLE_DETECTED                         -494    ///< 0xE12 Double detected
#define BIO_ERROR_INVALID_PK_BUFFER_SIZE                -495    ///< 0xE11 Invalid Pk buffer size
#define BIO_ERROR_GET_PK_SIZE_FAILED                    -496    ///< 0xE10
#define BIO_ERROR_SET_PK_SIZE_FAILED                    -497    ///< 0xE0F
#define BIO_ERROR_CONVERT_PKMAT_TO_PKBASE_FAILED        -498    ///< 0xE0E
#define BIO_ERROR_INVALID_NORMALIZED_FLAG               -499    ///< 0xE0D
#define BIO_ERROR_BAD_PARAMETER                         -500    ///< 0xE0C
#define BIO_ERROR_NORMALIZATION_FAILED                  -501    ///< 0xE0B
#define BIO_T_BIO_TEMPLATE_TOO_SMALL                    -502    ///< 0xE0A Template too small
#define BIO_MIXED_PK                                    -503    ///< 0xE09
#define BIO_EXPORT_SCORE_FORBIDDEN                      -504    ///< 0xE08 Export Score is forbidden

#define ILV_ERROR_INVALID_ILV                           -600    ///< 0xDA8 Invalid ILV
#define ILV_ERROR_BAD_ILV_SIZE                          -601    ///< 0xDA7 Bad ILV Size
#define ILV_ERROR_NOT_ENOUGH_MEMORY                     -602    ///< 0xDA6 Not enough memory
#define ILVERR_CARD_NOT_FOUND                           -603    ///< 0xDA5 Card not found
#define ILVERR_COM_ERROR                                -604    ///< 0xDA4 Communication error

#define ERROR_APPLI_BAD_PARAMETER                       -330    ///< 0xEB6
#define ERROR_APPLI_BAD_I_VALUE                         -341    ///< 0xEAB
#define ERROR_APPLI_ILV_BAD_PARAMETERS                  -344    ///< 0xEA8
#define STATUS_ERR_REF                                  -2455   ///< 0x669

/***********************************************************************
RS232 Errors
***********************************************************************/
#define RS232_OK                         0
#define RS232ERR_ERROR                  -520
#define RS232ERR_INIT                   -522
#define RS232ERR_BADPARAM               -529
#define RS232ERR_WRITE_TIMEOUT          -552
#define RS232ERR_READ_TIMEOUT           -553

#define SPRS232_OK                       0
#define SPRS232_ERR_ERROR               -560
#define SPRS232_ERR_INIT                -561
#define	SPRS232_BAD_STUFFING            -562
#define SPRS232_ERR_OPERATION_ABORTED   -563
#define SPRS232_ERR_BADPARAM            -564
#define SPRS232_ERR_BADSTX              -565
#define SPRS232_ERR_BADDATA             -566
#define SPRS232_ERR_BADID               -567
#define SPRS232_ERR_CRC                 -568
#define SPRS232_ERR_EOP                 -570
#define SPRS232_FRAME_NOT_COMPLETE      -571
#define SPRS232_ERR_CONFIG_UART         -572
#define SPRS232_ERR_SMALL_BUF           -573

#define SPRS232_ERR_FIRST                SPRS232_ERR_ERROR
#define SPRS232_ERR_LAST                 SPRS232_ERR_SMALL_BUF
			

/****************************************************
USB Errors
*****************************************************/
#define SPUSB_OK                         0        ///< 0x000 succesful operation
#define SPUSB_ERR_TIMEOUT               RS232ERR_READ_TIMEOUT  ///< 0xE79 timeout
#define SPUSB_NO_MEMORY                 -394      ///< 0xE76 memory allocation error
#define SPUSB_ERR_FRAME_LEN             -401      ///< 0xE6F Frame integrity error
#define SPUSB_ERR                       -402      ///< 0xE6E Error
#define SPUSB_DEVICE_NOT_PRESENT        -403      ///< 0xE6D The device desire is not plug in the system
#define SPUSB_BAD_PARAMETER             -412      ///< 0xE64


/***********************************************************************
 PC COM Errors
***********************************************************************/
#define COM_ERR_NOT_OPEN                -430    ///< 0xE52
#define COM_ERR_INVALID_REPLAY_COUNT    -431    ///< 0xE51
#define COM_BUSY                        -433    ///< 0xE4F
#define COM_STOP                        -434    ///< 0xE4E
#define COM_RECEIVE_ILV_INVALID         -437    ///< 0xE4B
#define COM_ERROR_SYNCHRO_I             -438    ///< 0xE4A
#define COM_ERR                         -439    ///< 0xE49

#define MSO_COM_OPEN_BAD_PARAMETER      -1004	///< 0xFC14
#define	MSO_BAD_PARAMETER               -1008	///< 0xFC10
#define	MSO_ERROR_MEMORY                -1010	///< 0xFC0E

#endif	

