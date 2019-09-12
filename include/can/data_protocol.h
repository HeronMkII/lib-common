/*
CAN Data Protocol

CAN message structure is based on document: https://utat-ss.readthedocs.io/en/master/our-protocols/can.html
Data formats are based on document: https://utat-ss.readthedocs.io/en/master/our-protocols/data-conversion.html
*/

#ifndef CAN_DATA_PROTOCOL_H
#define CAN_DATA_PROTOCOL_H


// Message Types

// EPS housekeeping data
#define CAN_EPS_HK      0x01
// EPS control
#define CAN_EPS_CTRL    0x02
// PAY housekeeping data
#define CAN_PAY_HK      0x03
// PAY optical data
#define CAN_PAY_OPT     0x04
// PAY control
#define CAN_PAY_CTRL    0x05


// Field Numbers

// OBC housekeeping
#define CAN_OBC_HK_UPTIME           0
#define CAN_OBC_HK_RESTART_COUNT    1
#define CAN_OBC_HK_RESTART_REASON   2
#define CAN_OBC_HK_RESTART_DATE     3
#define CAN_OBC_HK_RESTART_TIME     4
#define CAN_OBC_HK_FIELD_COUNT      5  // Number of fields

// EPS housekeeping
#define CAN_EPS_HK_BAT_VOL          0
#define CAN_EPS_HK_BAT_CUR          1
#define CAN_EPS_HK_X_POS_CUR        2
#define CAN_EPS_HK_X_NEG_CUR        3
#define CAN_EPS_HK_Y_POS_CUR        4
#define CAN_EPS_HK_Y_NEG_CUR        5
#define CAN_EPS_HK_3V3_VOL          6
#define CAN_EPS_HK_3V3_CUR          7
#define CAN_EPS_HK_5V_VOL           8
#define CAN_EPS_HK_5V_CUR           9
#define CAN_EPS_HK_PAY_CUR          10
#define CAN_EPS_HK_BAT_TEMP1        11
#define CAN_EPS_HK_BAT_TEMP2        12
#define CAN_EPS_HK_3V3_TEMP         13
#define CAN_EPS_HK_5V_TEMP          14
#define CAN_EPS_HK_PAY_CON_TEMP     15
#define CAN_EPS_HK_SHUNTS           16
#define CAN_EPS_HK_HEAT1_SP         17
#define CAN_EPS_HK_HEAT2_SP         18
#define CAN_EPS_HK_GYR_UNCAL_X      19
#define CAN_EPS_HK_GYR_UNCAL_Y      20
#define CAN_EPS_HK_GYR_UNCAL_Z      21
#define CAN_EPS_HK_GYR_CAL_X        22
#define CAN_EPS_HK_GYR_CAL_Y        23
#define CAN_EPS_HK_GYR_CAL_Z        24
#define CAN_EPS_HK_UPTIME           25
#define CAN_EPS_HK_RESTART_COUNT    26
#define CAN_EPS_HK_RESTART_REASON   27
#define CAN_EPS_HK_FIELD_COUNT      28  // Number of fields

// EPS control
#define CAN_EPS_CTRL_PING                       0
#define CAN_EPS_CTRL_GET_HEAT1_SHAD_SP          1
#define CAN_EPS_CTRL_SET_HEAT1_SHAD_SP          2
#define CAN_EPS_CTRL_GET_HEAT2_SHAD_SP          3
#define CAN_EPS_CTRL_SET_HEAT2_SHAD_SP          4
#define CAN_EPS_CTRL_GET_HEAT1_SUN_SP           5
#define CAN_EPS_CTRL_SET_HEAT1_SUN_SP           6
#define CAN_EPS_CTRL_GET_HEAT2_SUN_SP           7
#define CAN_EPS_CTRL_SET_HEAT2_SUN_SP           8
#define CAN_EPS_CTRL_GET_HEAT_CUR_THRESH_LOWER  9
#define CAN_EPS_CTRL_SET_HEAT_CUR_THRESH_LOWER  10
#define CAN_EPS_CTRL_GET_HEAT_CUR_THRESH_UPPER  11
#define CAN_EPS_CTRL_SET_HEAT_CUR_THRESH_UPPER  12
#define CAN_EPS_CTRL_RESET                      13
#define CAN_EPS_CTRL_READ_EEPROM                14
#define CAN_EPS_CTRL_ERASE_EEPROM               15
#define CAN_EPS_CTRL_READ_RAM_BYTE              16
#define CAN_EPS_CTRL_START_TEMP_LPM             17
#define CAN_EPS_CTRL_ENABLE_INDEF_LPM           18
#define CAN_EPS_CTRL_DISABLE_INDEF_LPM          19

// PAY housekeeping
#define CAN_PAY_HK_HUM              0
#define CAN_PAY_HK_PRES             1
#define CAN_PAY_HK_AMB_TEMP         2
#define CAN_PAY_HK_MOT1_TEMP        3
#define CAN_PAY_HK_MOT2_TEMP        4
#define CAN_PAY_HK_10V_TEMP         5
#define CAN_PAY_HK_6V_TEMP          6
#define CAN_PAY_HK_MF1_TEMP         7
#define CAN_PAY_HK_MF2_TEMP         8
#define CAN_PAY_HK_MF3_TEMP         9
#define CAN_PAY_HK_MF4_TEMP         10
#define CAN_PAY_HK_MF5_TEMP         11
#define CAN_PAY_HK_MF6_TEMP         12
#define CAN_PAY_HK_MF7_TEMP         13
#define CAN_PAY_HK_MF8_TEMP         14
#define CAN_PAY_HK_MF9_TEMP         15
#define CAN_PAY_HK_MF10_TEMP        16
#define CAN_PAY_HK_MF11_TEMP        17
#define CAN_PAY_HK_MF12_TEMP        18
#define CAN_PAY_HK_HEAT_EN          19
#define CAN_PAY_HK_LIM_PRESS        20
#define CAN_PAY_HK_UPTIME           21
#define CAN_PAY_HK_RESTART_COUNT    22
#define CAN_PAY_HK_RESTART_REASON   23
#define CAN_PAY_HK_FIELD_COUNT      24  // Number of fields

// PAY optical
#define CAN_PAY_OPT_FIELD_COUNT 32  // Number of fields

// PAY control
#define CAN_PAY_CTRL_PING               0
#define CAN_PAY_CTRL_HEAT1_OFF          1
#define CAN_PAY_CTRL_HEAT1_ON           2
#define CAN_PAY_CTRL_HEAT2_OFF          3
#define CAN_PAY_CTRL_HEAT2_ON           4
#define CAN_PAY_CTRL_HEAT3_OFF          5
#define CAN_PAY_CTRL_HEAT3_ON           6
#define CAN_PAY_CTRL_HEAT4_OFF          7
#define CAN_PAY_CTRL_HEAT4_ON           8
#define CAN_PAY_CTRL_HEAT5_OFF          9
#define CAN_PAY_CTRL_HEAT5_ON           10
#define CAN_PAY_CTRL_DISABLE_6V         11
#define CAN_PAY_CTRL_ENABLE_6V          12
#define CAN_PAY_CTRL_DISABLE_10V        13
#define CAN_PAY_CTRL_ENABLE_10V         14
#define CAN_PAY_CTRL_ACT_UP             15
#define CAN_PAY_CTRL_ACT_DOWN           16
#define CAN_PAY_CTRL_BLIST_DEP_SEQ      17
#define CAN_PAY_CTRL_RESET              18
#define CAN_PAY_CTRL_READ_EEPROM        19
#define CAN_PAY_CTRL_ERASE_EEPROM       20
#define CAN_PAY_CTRL_READ_RAM_BYTE      21
#define CAN_PAY_CTRL_START_TEMP_LPM     22
#define CAN_PAY_CTRL_ENABLE_INDEF_LPM   23
#define CAN_PAY_CTRL_DISABLE_INDEF_LPM  24

#endif
