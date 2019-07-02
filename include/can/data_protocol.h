/*
CAN Data Protocol

CAN message structure is based on document: https://utat-ss.readthedocs.io/en/master/our-protocols/can.html
Data formats are based on document: https://utat-ss.readthedocs.io/en/master/our-protocols/data-conversion.html
*/

#ifndef CAN_DATA_PROTOCOL_H
#define CAN_DATA_PROTOCOL_H


// Byte 0: Sender and Receiver

// Boards
#define CAN_OBC   0b00
#define CAN_PAY   0b01
#define CAN_EPS   0b10


// Byte 1: Message Type

// EPS housekeeping data
#define CAN_EPS_HK      0x00
// EPS control
#define CAN_EPS_CTRL    0x01
// PAY housekeeping data
#define CAN_PAY_HK      0x02
// PAY optical data
#define CAN_PAY_OPT     0x03
// PAY control
#define CAN_PAY_CTRL    0x04


// Byte 2: Field Number

// EPS housekeeping
// 0 to 11: field number = ADC channel number
#define CAN_EPS_HK_BB_VOL           0
#define CAN_EPS_HK_BB_CUR           1
#define CAN_EPS_HK_NY_CUR           2
#define CAN_EPS_HK_PX_CUR           3
#define CAN_EPS_HK_PY_CUR           4
#define CAN_EPS_HK_NX_CUR           5
#define CAN_EPS_HK_BAT_TEMP1        6
#define CAN_EPS_HK_BAT_TEMP2        7
#define CAN_EPS_HK_BAT_VOL          8
#define CAN_EPS_HK_BAT_CUR          9
#define CAN_EPS_HK_BT_CUR           10
#define CAN_EPS_HK_BT_VOL           11
#define CAN_EPS_HK_HEAT_SP1         12
#define CAN_EPS_HK_HEAT_SP2         13
#define CAN_EPS_HK_GYR_UNCAL_X      14
#define CAN_EPS_HK_GYR_UNCAL_Y      15
#define CAN_EPS_HK_GYR_UNCAL_Z      16
#define CAN_EPS_HK_GYR_CAL_X        17
#define CAN_EPS_HK_GYR_CAL_Y        18
#define CAN_EPS_HK_GYR_CAL_Z        19
#define CAN_EPS_HK_HEAT_SHADOW_SP1  20
#define CAN_EPS_HK_HEAT_SHADOW_SP2  21
#define CAN_EPS_HK_HEAT_SUN_SP1     22
#define CAN_EPS_HK_HEAT_SUN_SP2     23
// Number of fields
#define CAN_EPS_HK_FIELD_COUNT      24

// EPS control
#define CAN_EPS_CTRL_PING                   0
#define CAN_EPS_CTRL_HEAT_SP1               1   // TODO - remove
#define CAN_EPS_CTRL_HEAT_SP2               2   // TODO - remove
#define CAN_EPS_CTRL_HEAT_SHADOW_SP1        1
#define CAN_EPS_CTRL_HEAT_SHADOW_SP2        2
#define CAN_EPS_CTRL_HEAT_SUN_SP1           3
#define CAN_EPS_CTRL_HEAT_SUN_SP2           4
#define CAN_EPS_CTRL_HEAT_CUR_THRESH_LOWER  5
#define CAN_EPS_CTRL_HEAT_CUR_THRESH_UPPER  6
#define CAN_EPS_CTRL_RESET                  7
#define CAN_EPS_CTRL_READ_EEPROM            8
#define CAN_EPS_CTRL_ERASE_EEPROM           9
#define CAN_EPS_CTRL_RESTART_COUNT          10
#define CAN_EPS_CTRL_RESTART_REASON         11
#define CAN_EPS_CTRL_UPTIME                 12

// PAY housekeeping
#define CAN_PAY_HK_TEMP         0
#define CAN_PAY_HK_HUM          1
#define CAN_PAY_HK_PRES         2
#define CAN_PAY_HK_THERM0       3
#define CAN_PAY_HK_THERM1       4
#define CAN_PAY_HK_THERM2       5
#define CAN_PAY_HK_THERM3       6
#define CAN_PAY_HK_THERM4       7
#define CAN_PAY_HK_THERM5       8
#define CAN_PAY_HK_THERM6       9
#define CAN_PAY_HK_THERM7       10
#define CAN_PAY_HK_THERM8       11
#define CAN_PAY_HK_THERM9       12
#define CAN_PAY_HK_HEAT_SP1     13
#define CAN_PAY_HK_HEAT_SP2     14
#define CAN_PAY_HK_PROX_LEFT    15
#define CAN_PAY_HK_PROX_RIGHT   16
// Number of fields
#define CAN_PAY_HK_FIELD_COUNT  17

// PAY optical
// Field Number = Well number (0 to 31)
// Number of fields
#define CAN_PAY_OPT_FIELD_COUNT 32

// PAY control
#define CAN_PAY_CTRL_PING           0
#define CAN_PAY_CTRL_HEAT_SP1       1
#define CAN_PAY_CTRL_HEAT_SP2       2
#define CAN_PAY_CTRL_ACT_UP         3
#define CAN_PAY_CTRL_ACT_DOWN       4
#define CAN_PAY_CTRL_RESET          5
#define CAN_PAY_CTRL_READ_EEPROM    6
#define CAN_PAY_CTRL_ERASE_EEPROM   7
#define CAN_PAY_CTRL_RESTART_COUNT  8
#define CAN_PAY_CTRL_RESTART_REASON 9
#define CAN_PAY_CTRL_UPTIME         10

#endif
