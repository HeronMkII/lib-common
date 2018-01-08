/*
	FILENAME: 			global_defs.h
	DEPENDENCIES:		none

	DESCRIPTION:		Defines global definitions for use with CAN
	AUTHORS:			Dylan Vogel, Ali Haydaroglu
	DATE MODIFIED:		2018-01-08
	NOTE:

	REVISION HISTORY:

		2018-01-08:		DV: Added initial command IDs and PAY sensor IDs
*/

/*
################################################################################
 									EPS
################################################################################
*/

#define EPS_HK_REQ 			0b00000001
#define EPS_HK_SENSOR_REQ 	0b00000010

// EPS SENSOR IDs

/*
################################################################################
 									PAY
################################################################################
*/
#define PAY_HK_REQ			0b10000001
#define PAY_HK_SENSOR_REQ	0b10000010
#define PAY_SCI_REQ			0b10000011
#define PAY_SCI_SENSOR_REQ	0b10000100


// PAY SENSOR IDs
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
