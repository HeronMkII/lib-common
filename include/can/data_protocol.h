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
#define CAN_EPS_HK  0x00
// PAY housekeeping data
#define CAN_PAY_HK  0x01
// PAY optical data
#define CAN_PAY_OPT 0x02
// PAY experiment
#define CAN_PAY_EXP 0x03


// Byte 2: Field Number

// EPS housekeeping
// Field Number = ADC channel number (0 to 11)
// Should match with channel number constants in <adc/eps.h>
#define CAN_EPS_HK_BB_VOL       0
#define CAN_EPS_HK_BB_CUR       1
#define CAN_EPS_HK_BT_VOL       2
#define CAN_EPS_HK_BT_CUR       3
#define CAN_EPS_HK_PX_CUR       4
#define CAN_EPS_HK_NX_CUR       5
#define CAN_EPS_HK_PY_CUR       6
#define CAN_EPS_HK_NY_CUR       7
#define CAN_EPS_HK_BAT_VOL      8
#define CAN_EPS_HK_BAT_CUR      9
#define CAN_EPS_HK_BAT_TEMP1    10
#define CAN_EPS_HK_BAT_TEMP2    11
#define CAN_EPS_HK_IMU_ACC_X    12
#define CAN_EPS_HK_IMU_ACC_Y    13
#define CAN_EPS_HK_IMU_ACC_Z    14
#define CAN_EPS_HK_IMU_GYR_X    15
#define CAN_EPS_HK_IMU_GYR_Y    16
#define CAN_EPS_HK_IMU_GYR_Z    17
#define CAN_EPS_HK_IMU_MAG_X    18
#define CAN_EPS_HK_IMU_MAG_Y    19
#define CAN_EPS_HK_IMU_MAG_Z    20
#define CAN_EPS_HK_GET_DAC1     21
#define CAN_EPS_HK_GET_DAC2     22
#define CAN_EPS_HK_SET_DAC1     23
#define CAN_EPS_HK_SET_DAC2     24
// Number of get (not set) fields
#define CAN_EPS_HK_GET_COUNT 23

// PAY housekeeping
#define CAN_PAY_HK_TEMP         0 // Temperature
#define CAN_PAY_HK_HUM          1 // Humidity
#define CAN_PAY_HK_PRES         2 // Pressure
#define CAN_PAY_HK_THERM0       3 // First thermistor
// ... until Thermistor 9 -> field 12
#define CAN_PAY_HK_GET_DAC1     13
#define CAN_PAY_HK_GET_DAC2     14
#define CAN_PAY_HK_SET_DAC1     15
#define CAN_PAY_HK_SET_DAC2     16
// Number of get (not set) fields
#define CAN_PAY_HK_GET_COUNT    15

// PAY optical
// Field Number = Well number (0 to 35)
// Number of fields
#define CAN_PAY_SCI_GET_COUNT 36

// PAY experiment
// Left proximity
#define CAN_PAY_EXP_PROX_LEFT   0
// Right proximity
#define CAN_PAY_EXP_PROX_RIGHT  1
// Tell PAY to level the motors
#define CAN_PAY_EXP_LEVEL       2
// Tell PAY to actuate the motors
#define CAN_PAY_EXP_POP         3

#endif
