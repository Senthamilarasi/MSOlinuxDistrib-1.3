/* Refer to documentation for the meaning of commands */
#ifndef __ILV_DEFINITIONS_H
#define __ILV_DEFINITIONS_H

/* ------------------------------------------------------------
 * General commands
 */
#define ILV_GET_DESCRIPTOR          0x05

/* ------------------------------------------------------------
 * Biometric commands
 */
#define ILV_AUTHENTICATE            0x20
#define ILV_ENROLL                  0x21
#define ILV_IDENTIFY                0x22
#define ILV_VERIFY_MATCH            0x23
#define ILV_IDENTIFY_MATCH          0x24

/* ------------------------------------------------------------
 * DataBase commands
 */
#define ILV_GET_BASE_CONGIG         0x07
#define ILV_CREATE_DB               0x30
#define ILV_ERASE_BASE              0x32
#define ILV_ADD_RECORD				0x35
#define ILV_REMOVE_RECORD           0x36
#define ILV_DESTROY_DB              0x3B
#define ILV_LIST_PUBLIC_FIELDS      0x3E

#define ID_NO_CHECK_ON_TEMPLATE	    0x60	//96

/* ------------------------------------------------------------
 * Special ILV : answer to an invalid command
 */
#define ILV_INVALID                 0x50

/* ------------------------------------------------------------
 * Asynchronous messages
 */
#define ILV_CANCEL                  0x70
#define ILV_ASYNC_MESSAGE           0x71

/* ------------------------------------------------------------
 * Security
 */
#define ILV_SECU_GET_CONFIG        0x80

/* ------------------------------------------------------------
 * Special ILV
 */
#define ILV_CONFIG_UART            0xEE
#define ILV_EXTENDED               0xFE
#define ILV_ESCAPE                 0xFF


/* ------------------------------------------------------------
 * Optional ILV
 */
#define ID_USER_ID                  4             //0x04
#define ID_USER_DATA                5             //0x05
#define ID_COM1                     6             //0x06
#define ID_FIELD                    15            //0x0F
#define ID_PUC_DATA                 20            //0x14
#define ID_DESC_PRODUCT             41            //0x29
#define ID_DESC_SOFTWARE            42            //0x2A
#define ID_DESC_SENSOR              43            //0x2B
#define	ID_PRIVATE_FIELD            49            //0x31
#define	ID_FIELD_CONTENT            50            //0x32
#define ID_ASYNCHRONOUS_EVENT       52            //0x34
#define	ID_USER_INDEX               54            //0x36
#define ID_BIO_ALGO_PARAM           56            //0x38
#define ID_EXPORT_IMAGE             61            //0x3D
#define	ID_COMPRESSION              62            //0x3E
#define	ID_PKM1                     63            //0x3F
#define	ID_PKM1_PARAM               64            //0x40
#define	ID_PKM1_DATA                65            //0x41
#define	ID_X984_PARAM               84            //0x54
#define	ID_X984                     85            //0x55
#define ID_MATCHING_SCORE           86            //0x56
#define	ID_TKB                      87            //0x57

//Conversion de PK normalise
#define ID_ISO_PK                   ID_PKM1        //0x3F
#define ID_ISO_PK_PARAM             ID_PKM1_PARAM  //0x40
#define ID_ISO_PK_DATA_ANSI_378     ID_PKM1_DATA   //0x41
#define ID_ISO_PK_DATA_ILO_FMR      107            //0x6B
#define ID_ISO_PK_DATA_ISO_FMC_CS   108            //0x6C
#define ID_ISO_PK_DATA_ISO_FMC_NS   109            //0x6D
#define ID_ISO_PK_DATA_ISO_FMR      110            //0x6E
#define ID_ISO_PK_DATA_MINEX_A      111            //0x6F


#define ID_COMPRESSION_NULL      44      ///< No image compression
#define ID_COMPRESSION_HUFFMAN   51      ///< Huffman compression
#define ID_COMPRESSION_V1        60      ///< Huffman compression V1
#define ID_COMPRESSION_WSQ		156 	 ///< WSQ compresion


#endif /*__ILV_DEFINITIONS_H*/

