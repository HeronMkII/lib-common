/*
	FILENAME: 			can_ids.h
	DEPENDENCIES:		none

	DESCRIPTION:		Defines global definitions for use with CAN
	AUTHORS:			Dylan Vogel, Ali Haydaroglu
	DATE MODIFIED:		2018-01-12
	NOTE:

	REVISION HISTORY:

	2018-01-12:			DV: Fixed bugs, wrotes out binary strting under hex string
	2018-01-08:			DV: Added initial command IDs and PAY sensor IDs
*/

// GLOBAL RX MASK
#define CAN_RX_MASK_ID		{ 0x07F8 }
// 0b0 111 1111 1000


/*
################################################################################
 						EPS
################################################################################
*/
// EPS MOB IDS
#define EPS_STATUS_RX_MOB_ID	{ 0x0409 }
// 0b0 100 0000 1001
#define EPS_STATUS_TX_MOB_ID	{ 0x0014 }
// 0b0 000 0001 0100
#define EPS_CMD_TX_MOB_ID		{ 0x0024 }
// 0b0 000 0010 0100
#define EPS_CMD_RX_MOB_ID		{ 0x0481 }
// 0b0 100 1000 0001
#define EPS_DATA_TX_MOB_ID		{ 0x0104 }
// 0b0 001 0000 0100

// EPS COMMAND IDS
#define EPS_HK_REQ 			0b00000001
#define EPS_HK_SENSOR_REQ 	0b00000010

// EPS SENSOR IDS

/*
################################################################################
 						PAY
################################################################################
*/

// PAY MOB IDS
#define PAY_STATUS_RX_MOB_ID	{ 0x0209 }	// RX MObs should have reciever ID of board
//0b 0 010 0000 1001
#define PAY_STATUS_TX_MOB_ID	{ 0x0012 }
//0b 0 000 0001 0010
#define PAY_CMD_TX_MOB_ID		{ 0x0022 }
// 0b 0 000 0010 0010
#define PAY_CMD_RX_MOB_ID		{ 0x0241 }
// 0b 0 010 0100 0001
#define PAY_DATA_TX_MOB_ID		{ 0x0102 }
// 0b 0 001 0000 0010

// PAY COMMAND IDS
#define PAY_HK_REQ			0b10000001
#define PAY_HK_SENSOR_REQ	0b10000010
#define PAY_SCI_REQ			0b10000011
#define PAY_SCI_SENSOR_REQ	0b10000100


// PAY SENSOR IDS
#define PAY_TEMP_1			0b00000000
#define PAY_PRES_1			0b00000001
#define PAY_HUMID_1			0b00000010
#define PAY_MF_TEMP_1		0b00000011
#define PAY_MF_TEMP_2		0b00000100
#define	PAY_MF_TEMP_3		0b00000101

// Well numbering starts at 0 and counts up to 32
#define PAY_WELL_OD_1		0b01000000
#define PAY_WELL_FL_1		0b11000000

#define PAY_WELL_OD_33		0b01100000
#define PAY_WELL_FL_33		0b11100000
