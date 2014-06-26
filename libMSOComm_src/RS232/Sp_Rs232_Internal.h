#ifndef __SP_RS232_INTERNAL_H
#define __SP_RS232_INTERNAL_H

/*******************************************************************************
-- definition
*******************************************************************************/
//reception states
#define SPRS232_WAIT_STX				0
#define SPRS232_WAIT_ID					1
#define SPRS232_WAIT_RC					2
#define SPRS232_WAIT_RC_AFTER_DLE			3
#define SPRS232_WAIT_DATA				4
#define SPRS232_WAIT_DATA_AFTER_DLE_OR_ETX		5
#define SPRS232_WAIT_READ_PACKET			6

//transmission states
#define SPRS232_TX_READY				0
#define SPRS232_TX_WAIT_ACK				1

#define DLE						0x1B
#define STX						0x02
#define ETX						0x03
#define XON						0x11
#define XOFF						0x13

#define DIR_IN						0x80
#define DIR_OUT						0x00

#define SPRS232_ID_ID_PACKET_DATA			0x01
#define SPRS232_ID_ID_PACKET_ACK			0x02
#define SPRS232_ID_ID_PACKET_NACK			0x04
#define SPRS232_ID_ID_PACKET_RFU			0x08	/* Always 0 */
#define SPRS232_ID_MBZ					0x10	/* Must be ZERO to avoid ID to be egal to DLE */
#define SPRS232_ID_TYP_PACKET_LAST			0x20
#define SPRS232_ID_TYP_PACKET_FIRST			0x40
#define SPRS232_ID_TYP_PACKET_DIR			0x80

#define SPRS232_MASK_PACKET_TYPE			(SPRS232_ID_ID_PACKET_DATA|SPRS232_ID_ID_PACKET_ACK|SPRS232_ID_ID_PACKET_NACK|SPRS232_ID_ID_PACKET_RFU|SPRS232_ID_MBZ)

#define	SPRS232_FRAME_PARTIAL				0
#define SPRS232_FRAME_COMPLETE				1

#define SPRS232_TIMEOUT_INFINITE			0xFFFFFFFF
#define SPRS232_TIMEOUT_INTERVAL 			100
#define SPRS232_TIMEOUT_ACK				500

#define MAX_NUMBER_OF_ATTENPT				3

#endif /*__SP_RS232_INTERNAL_H*/

