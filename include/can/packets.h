/* OBC to PAY CAN packet transmission format */

/*
Each CAN transmission can have up to 8 bytes of data.

The first 2 bytes have the packet and field (defining the data transmitted),
followed by up to 6 bytes of the actual data.

The 2 byte identification is sent both when the request is sent from OBC to PAY
and when the response is sent from PAY to OBC, in order to identify the field in the packet.

Currently, all sensor readings are transmitted and stored as 3 bytes values,
so currently each transmission is 5 bytes.
*/




/*
2 Bytes: Identification
*/


/*
Byte 0: packet
(housekeeping or science data)
*/

// PAY COMMAND IDS
#define PAY_HK_REQ          0b10000001
#define PAY_SCI_REQ         0b10000011


/*
Byte 1: field
(within the packet)
Indicates which sensor to poll
*/

// TODO - should the defined macros start at 0?

// PAY SENSOR IDS
// For housekeeping data
#define PAY_TEMP_1          0b00000000
#define PAY_PRES_1          0b00000001
#define PAY_HUMID_1         0b00000010
#define PAY_MF_TEMP_1       0b00000011
#define PAY_MF_TEMP_2       0b00000100
#define PAY_MF_TEMP_3       0b00000101

/*
For science data
OD and FL measurements are in separate fields

TODO - standardize well numbering
TODO - implement reading/triplicate number in transmission (1, 2, or 3)

Bit 0: Always 0
Bit 1: OD (0) or FL (1)
Bits 2-7: Well index (0-32)
*/
// #define PAY_WELL_OD_1    0b01000000
// #define PAY_WELL_FL_1    0b11000000
// #define PAY_WELL_OD_33   0b01100000
// #define PAY_WELL_FL_33   0b11100000
#define PAY_WELL_OD_BASE    0b00000000
#define PAY_WELL_FL_BASE    0b01000000
/*
To specify the well number, add the zero-based index of the well to this value, e.g.
Well 1 (first), OD: 0b00000000
Well 11, FL:        0b01001011
Well 33 (last), FL: 0b01100000
*/




/*
Up to 6 bytes: data (currently always 3 bytes)
For one particular sensor measurement
*/

/*
1. Temperature
14 bit sensor reading -> zero padded
(data placed into 14 least significant bits, aligned to the right of the 3 bytes)
*/

/*
2. Pressure
24 bit sensor reading
*/

/*
3. Humidity
14 bit sensor reading -> zero padded
(data placed into 14 least significant bits, aligned to the right of the 3 bytes)
*/

/*
4. MF Temperature (thermistors)
12 bit ADC reading -> zero padded
(data placed into 12 least significant bits, aligned to the right of the 3 bytes)
TODO - confirm the number of thermistors
*/

/*
5. Well OD (optical density)
24 bit ADC reading
*/

/*
6. Well FL (fluorescence)
24 bit ADC reading
*/
