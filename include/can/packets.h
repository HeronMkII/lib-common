/* OBC to PAY CAN packet transmission format */

/*
Each CAN transmission has 8 bytes of data.
The first 2 bytes have the packet command and field (defining the data transmitted),
followed by 6 bytes of the actual data.
The 2 byte identification is sent both when the request is sent from OBC to PAY
and when the response is sent from PAY to OBC, in order to identify the field in the overall packet.
*/




/*
2 Bytes: Identification
*/


/*
Byte 0: packet command
(housekeeping or science data)
TODO - is the "REQ" indicating that it will start requesting data (so prepare),
and the "SENSOR_REQ" requesting data for a specific sensor?
*/

// PAY COMMAND IDS
#define PAY_HK_REQ			0b10000001
#define PAY_HK_SENSOR_REQ	0b10000010
#define PAY_SCI_REQ			0b10000011
#define PAY_SCI_SENSOR_REQ	0b10000100


/*
Byte 1: field
(within the entire packet)
Indicates which sensor to poll
*/

// TODO - should the defined macros start at index 0?

// PAY SENSOR IDS
// For housekeeping data
#define PAY_TEMP_1			0b00000000
#define PAY_PRES_1			0b00000001
#define PAY_HUMID_1			0b00000010
#define PAY_MF_TEMP_1		0b00000011
#define PAY_MF_TEMP_2		0b00000100
#define	PAY_MF_TEMP_3		0b00000101

/*
For science data
Returns the OD and FL measurements in a single field
Well numbering starts at 0 and counts up to 32
*/
// #define PAY_WELL_OD_1		0b01000000
// #define PAY_WELL_FL_1		0b11000000
// #define PAY_WELL_OD_33		0b01100000
// #define PAY_WELL_FL_33		0b11100000
#define PAY_WELL_BASE       0b01000000
/*
To specify the well number, add the zero-based index of the well to this value, e.g.
Well 1 (first): 0b01000000
Well 11:        0b01001011
Well 33 (last): 0b01100000
*/




/*
6 Bytes: Data
Either housekeeping or science data
*/

/*
1. Temperature
Bytes 2-3 (2 bytes) - 14 bit sensor reading
*/

/*
2. Pressure
Bytes 2-5 (4 bytes)
// TODO - is it a 32 bit sensor reading?
*/

/*
3. Humidity
Bytes 2-3 (2 bytes) - 14 bit sensor reading
*/

/*
4. MF Temperature
TODO - is it the same as normal temperature?
*/

/*
5. Well (OD and FL)
Bytes 2-4 (3 bytes) - optical density (24 bit ADC reading)
Bytes 5-7 (3 bytes) - fluorescence (24 bit ADC reading)
*/
