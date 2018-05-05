/*
OBC-PAY CAN packet transmission format

Each CAN transmission can have up to 8 bytes of data.
For now, we may want to transmit all 8 bytes for simplicity and ignore the unused bytes.


Byte 0: Sending Board (which PCB is sending the message)

Byte 1: Message Type (broad classification)

Byte 2: Field Number (specifics for message type)
- e.g. which sensor to poll
- e.g. which heater to control

Bytes 3-5: Data (only when PAY sends a response message to OBC)
- If the data is smaller than 24 bits, it is right-aligned to the least
  significant bits with padding zeros added on the left
  - e.g. 14-bit value   xxxxxx xxxxxxxx
      -> 24-bit value   00000000 00xxxxxx xxxxxxxx


Bytes 0-2 are sent when the request is sent from OBC to PAY and when the
response is sent from PAY to OBC. This is to match the request message with the
response message.

TODO - maybe add an ACK (acknowledge) bit when PAY responds to OBC, indicating
       whether the message was valid and understood?
TODO - standardize numbering system for constants
TODO - create science measurement numbering system with fields starting at 0
       (well number, OD/FL, triplicate)
*/




// Byte 0: Sending Board
#define BOARD_OBC           0x00
#define BOARD_PAY           0x01
#define BOARD_EPS           0x02




// Byte 1: Message Type

// Housekeeping data (environmental sensors, e.g. temperature, humidity, pressure)
#define PAY_HK_REQ          0x00
// Science data (optical sensors)
#define PAY_SCI_REQ         0x01
// TODO - Heater control
#define PAY_HEATER_REQ      0x02




// Byte 2: Field Number


// Housekeeping

// Temperature
#define PAY_TEMP_1          0x00
// Humidity
#define PAY_HUMID_1         0x01

// TODO
#define PAY_PRES_1          0x02
#define PAY_MF_TEMP_1       0x03
#define PAY_MF_TEMP_2       0x04
#define PAY_MF_TEMP_3       0x05


// Science

// For testing with the 2 optical sensors currently on the PAY sensor PCB
#define PAY_SCI_TEMD        0x00
#define PAY_SCI_SFH         0x01




// Bytes 3-5: Data

/*
Possible data types:

1. Temperature
- 14 bit sensor reading

2. Humidity
- 14 bit sensor reading

3. Pressure - TODO
- 24 bit sensor reading
- TODO - figure out how to account for temperature corrections

4. MF Temperature (thermistors) - TODO
- 12 bit ADC reading

5. Well FL (fluorescence) or OD (optical density)
- 24 bit ADC reading
*/
