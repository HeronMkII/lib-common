/*
OBC-PAY CAN packet transmission format


Each CAN transmission can have up to 8 bytes of data.
For now, transmit all 8 bytes for simplicity and ignore the unused bytes.


Byte 0: Message Type (broad classification)

Byte 1: Field Number (specifics for message type)
- e.g. which sensor to poll
- e.g. which heater to control

Bytes 2-4: Data (for response only, when PAY sends a message to OBC)
- If data is smaller than 24 bits, it is right-aligned to the least significant bits


Bytes 0-1 are sent both when the request is sent from OBC to PAY and when the
response is sent from PAY to OBC. This is to identify which data is being
transmitted.

TODO - maybe add ACK bit when PAY responds to OBC, indicating the message was valid or not?
TODO - standardize numbering system for constants
     - should numbering start at 0?
     - one-hot encoding?
*/




// Byte 0: Message Type


// Housekeeping data (environmental sensors, e.g. temperature, humidity, pressure)
#define PAY_HK_REQ          0b00000000
// Science data (optical sensors)
#define PAY_SCI_REQ         0b00000001
// Heater control (TODO)
#define PAY_HEATER_REQ      0b00000010




// Byte 1: Field Number

// Housekeeping

// Working
#define PAY_TEMP_1          0b00000000
#define PAY_HUMID_1         0b00000001

// TODO
#define PAY_PRES_1          0b00000010
#define PAY_MF_TEMP_1       0b00000011
#define PAY_MF_TEMP_2       0b00000100
#define PAY_MF_TEMP_3       0b00000101

// Science
/*
TODO - create well numbering system with fields starting at 0
TODO - well number, OD/FL, triplicate number
*/

// These are just for testing with the 2 optical sensors on the PAY sensor PCB
#define PAY_SCI_TEMD        0b00000000
#define PAY_SCI_SFH         0b00000001




// Bytes 2-4: Data

/*
Possible data types:

1. Temperature
- 14 bit sensor reading -> zero padded
- Data in the 14 least significant bits, aligned to the right of the 3 bytes

2. Pressure - TODO
- 24 bit sensor reading
- TODO - figure out how to account for temperature corrections

3. Humidity
- 14 bit sensor reading -> zero padded
- Data in the 14 least significant bits, aligned to the right of the 3 bytes

4. MF Temperature (thermistors) - TODO
- 12 bit ADC reading -> zero padded
- Data in the 12 least significant bits, aligned to the right of the 3 bytes

5. Well FL (fluorescence) or OD (optical density)
- 24 bit ADC reading
*/
