#ifndef CAN_DATA_PROTOCOL_H
#define CAN_DATA_PROTOCOL_H

/*
CAN Data Protocol

Subsystems/Microcontrollers:
OBC - On Board Computer (main)
PAY - Payload
EPS - Electrical Power Systems

This protocol defines how CAN messages are structured for data requests between
OBC, PAY, and EPS. This is how OBC sends a message to either PAY or EPS to
request a particular piece of data, and how either PAY or EPS responds with the
appropriate data.

Each CAN message can have up to 8 bytes of data. Currently, always transmit
all 8 bytes for simplicity and ignore the unused bytes. We should revisit this
later and consider using variable length messages.

Bytes within a CAN message are numbered so that the first byte sent is Byte 0
and the last byte sent is Byte 7.
Bits within a byte are numbered so that the rightmost (least significant) bit is Bit 0
and the leftmost (most significant) bit is Bit 7.


Byte 0: Sender and Receiver - can be useful to identify messages
- Bits 3-2 - board sending the message
- Bits 1-0 - board receiving the message

Byte 1: Message Type
- type of message

Byte 2: Field Number
- specifies the number (index) of the request made
- always numbered sequentially starting at 0
- e.g. which sensor to poll

Bytes 3-5: Data (when applicable, generally only when PAY/EPS responds to OBC)
- If the data is smaller than 24 bits, it is right-aligned to the least
  significant bits with padding zeros added on the left
    - e.g. a 12-bit value from an ADC (xxxx xxxxxxxx)
      is sent as a 24-bit value (00000000 0000xxxx xxxxxxxx)
- If there is no data to be sent (e.g. for a motor control request), ignore this
    - Sending a response message with the appropriate Bytes 0-2 acts as a
      confirmation that the action has been performed

Bytes 1-2 are identical both when the request is sent from OBC to PAY/EPS and when
the response is sent from PAY/EPS to OBC. This is so OBC can match the request
message with the response message and verify it is receiving the correct message.
*/




// Byte 0: Sender and Receiver
#define CAN_BOARD_OBC   0b00
#define CAN_BOARD_PAY   0b01
#define CAN_BOARD_EPS   0b10




// Byte 1: Message Type

// PAY housekeeping data (environmental sensors, e.g. temperature, humidity, pressure)
#define CAN_PAY_HK      0x00
// PAY science data (optical sensors)
#define CAN_PAY_SCI     0x01
// PAY motor actuation
#define CAN_PAY_MOTOR   0x02
// EPS housekeeping data (voltage and current measurements)
#define CAN_EPS_HK      0x03




// Byte 2: Field Number

// PAY housekeeping
#define CAN_PAY_HK_TEMP         0x00 // Temperature
#define CAN_PAY_HK_HUMID        0x01 // Humidity
#define CAN_PAY_HK_PRES         0x02 // Pressure
#define CAN_PAY_HK_FIELD_COUNT  3    // Number of fields

// PAY science
// Field Number = Well number (0 to 32)
#define CAN_PAY_SCI_FIELD_COUNT 33    // Number of fields

// PAY motor actuation
// Tell PAY to actuate the motor
#define CAN_PAY_MOTOR_ACTUATE 0

// EPS housekeeping
// Field Number = ADC channel number (0 to 11)
// See <adc/adc.h> for channel number constants
#define CAN_EPS_HK_FIELD_COUNT 12   // Number of fields




// Bytes 3-5: Data

/*
Possible data:

CAN_PAY_HK_TEMP - 14 bit sensor reading

CAN_PAY_HK_HUMID - 14 bit sensor reading

CAN_PAY_HK_PRES - 24 bit sensor reading

CAN_PAY_SCI_* - 24 bit ADC reading

CAN_EPS_HK_* - 12 bit ADC reading
*/

#endif
