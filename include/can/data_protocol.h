/*
CAN Data Protocol
*/

#ifndef CAN_DATA_PROTOCOL_H
#define CAN_DATA_PROTOCOL_H


// Message Types

#define CAN_EPS_HK      0x01
#define CAN_EPS_CTRL    0x02
#define CAN_PAY_HK      0x03
#define CAN_PAY_OPT     0x04
#define CAN_PAY_CTRL    0x05


// Field Numbers

// OBC housekeeping
#define CAN_OBC_HK_UPTIME           0x00
#define CAN_OBC_HK_RESTART_COUNT    0x01
#define CAN_OBC_HK_RESTART_REASON   0x02
#define CAN_OBC_HK_RESTART_DATE     0x03
#define CAN_OBC_HK_RESTART_TIME     0x04
#define CAN_OBC_HK_FIELD_COUNT      0x05  // Number of fields

// EPS housekeeping
#define CAN_EPS_HK_UPTIME           0x00
#define CAN_EPS_HK_RESTART_COUNT    0x01
#define CAN_EPS_HK_RESTART_REASON   0x02
#define CAN_EPS_HK_BAT_VOL          0x03
#define CAN_EPS_HK_BAT_CUR          0x04
#define CAN_EPS_HK_X_POS_CUR        0x05
#define CAN_EPS_HK_X_NEG_CUR        0x06
#define CAN_EPS_HK_Y_POS_CUR        0x07
#define CAN_EPS_HK_Y_NEG_CUR        0x08
#define CAN_EPS_HK_3V3_VOL          0x09
#define CAN_EPS_HK_3V3_CUR          0x0A
#define CAN_EPS_HK_5V_VOL           0x0B
#define CAN_EPS_HK_5V_CUR           0x0C
#define CAN_EPS_HK_PAY_CUR          0x0D
#define CAN_EPS_HK_3V3_TEMP         0x0E
#define CAN_EPS_HK_5V_TEMP          0x0F
#define CAN_EPS_HK_PAY_CON_TEMP     0x10
#define CAN_EPS_HK_BAT_TEMP1        0x11
#define CAN_EPS_HK_BAT_TEMP2        0x12
#define CAN_EPS_HK_HEAT1_SP         0x13
#define CAN_EPS_HK_HEAT2_SP         0x14
#define CAN_EPS_HK_GYR_UNCAL_X      0x15
#define CAN_EPS_HK_GYR_UNCAL_Y      0x16
#define CAN_EPS_HK_GYR_UNCAL_Z      0x17
#define CAN_EPS_HK_GYR_CAL_X        0x18
#define CAN_EPS_HK_GYR_CAL_Y        0x19
#define CAN_EPS_HK_GYR_CAL_Z        0x1A
#define CAN_EPS_HK_FIELD_COUNT      0x1B  // Number of fields

// EPS control
#define CAN_EPS_CTRL_PING                   0x00
#define CAN_EPS_CTRL_READ_EEPROM            0x01
#define CAN_EPS_CTRL_ERASE_EEPROM           0x02
#define CAN_EPS_CTRL_READ_RAM_BYTE          0x03
#define CAN_EPS_CTRL_RESET                  0x04
#define CAN_EPS_CTRL_GET_HEAT_SHAD_SP       0x05
#define CAN_EPS_CTRL_SET_HEAT1_SHAD_SP      0x06
#define CAN_EPS_CTRL_SET_HEAT2_SHAD_SP      0x07
#define CAN_EPS_CTRL_GET_HEAT_SUN_SP        0x08
#define CAN_EPS_CTRL_SET_HEAT1_SUN_SP       0x09
#define CAN_EPS_CTRL_SET_HEAT2_SUN_SP       0x0A
#define CAN_EPS_CTRL_GET_HEAT_CUR_THR       0x0B
#define CAN_EPS_CTRL_SET_HEAT_CUR_THR_LOWER 0x0C
#define CAN_EPS_CTRL_SET_HEAT_CUR_THR_UPPER 0x0D
#define CAN_EPS_CTRL_FIELD_COUNT            0x0E  // Number of fields

// PAY housekeeping
#define CAN_PAY_HK_UPTIME               0x00
#define CAN_PAY_HK_RESTART_COUNT        0x01
#define CAN_PAY_HK_RESTART_REASON       0x02
#define CAN_PAY_HK_HUM                  0x03
#define CAN_PAY_HK_PRES                 0x04
#define CAN_PAY_HK_AMB_TEMP             0x05
#define CAN_PAY_HK_6V_TEMP              0x06
#define CAN_PAY_HK_10V_TEMP             0x07
#define CAN_PAY_HK_MOT1_TEMP            0x08
#define CAN_PAY_HK_MOT2_TEMP            0x09
#define CAN_PAY_HK_MF1_TEMP             0x0A
#define CAN_PAY_HK_MF2_TEMP             0x0B
#define CAN_PAY_HK_MF3_TEMP             0x0C
#define CAN_PAY_HK_MF4_TEMP             0x0D
#define CAN_PAY_HK_MF5_TEMP             0x0E
#define CAN_PAY_HK_MF6_TEMP             0x0F
#define CAN_PAY_HK_MF7_TEMP             0x10
#define CAN_PAY_HK_MF8_TEMP             0x11
#define CAN_PAY_HK_MF9_TEMP             0x12
#define CAN_PAY_HK_MF10_TEMP            0x13
#define CAN_PAY_HK_MF11_TEMP            0x14
#define CAN_PAY_HK_MF12_TEMP            0x15
#define CAN_PAY_HK_HEAT_SP              0x16
#define CAN_PAY_HK_DEF_INV_THERM_TEMP   0x17
#define CAN_PAY_HK_THERM_EN             0x18
#define CAN_PAY_HK_HEAT_EN              0x19
#define CAN_PAY_HK_BAT_VOL              0x1A
#define CAN_PAY_HK_6V_VOL               0x1B
#define CAN_PAY_HK_6V_CUR               0x1C
#define CAN_PAY_HK_10V_VOL              0x1D
#define CAN_PAY_HK_10V_CUR              0x1E
#define CAN_PAY_HK_OPT_PWR              0x1F
#define CAN_PAY_HK_FIELD_COUNT          0x20  // Number of fields

// PAY optical
#define CAN_PAY_OPT_OD_FIELD_COUNT  0x20  // Optical density
#define CAN_PAY_OPT_FL_FIELD_COUNT  0x20  // Fluorescence
#define CAN_PAY_OPT_TOT_FIELD_COUNT 0x40  // Total number of fields

// PAY control
#define CAN_PAY_CTRL_PING                   0x00
#define CAN_PAY_CTRL_READ_EEPROM            0x01
#define CAN_PAY_CTRL_ERASE_EEPROM           0x02
#define CAN_PAY_CTRL_READ_RAM_BYTE          0x03
#define CAN_PAY_CTRL_RESET_SSM              0x04
#define CAN_PAY_CTRL_RESET_OPT              0x05
#define CAN_PAY_CTRL_ENABLE_6V              0x06
#define CAN_PAY_CTRL_DISABLE_6V             0x07
#define CAN_PAY_CTRL_ENABLE_10V             0x08
#define CAN_PAY_CTRL_DISABLE_10V            0x09
#define CAN_PAY_CTRL_GET_HEAT_PARAMS        0x0A
#define CAN_PAY_CTRL_SET_HEAT_SP            0x0B
#define CAN_PAY_CTRL_SET_DEF_INV_THERM_TEMP 0x0C
#define CAN_PAY_CTRL_GET_THERM_READING      0x0D
#define CAN_PAY_CTRL_GET_THERM_ERR_CODE     0x0E
#define CAN_PAY_CTRL_SET_THERM_ERR_CODE     0x0F
#define CAN_PAY_CTRL_MOTOR_DEP_ROUTINE      0x10
#define CAN_PAY_CTRL_GET_MOTOR_STATUS       0x11
#define CAN_PAY_CTRL_GET_LSW_STATUS         0x12
#define CAN_PAY_CTRL_MOTOR_UP               0x13
#define CAN_PAY_CTRL_MOTOR_DOWN             0x14
#define CAN_PAY_CTRL_SEND_OPT_SPI           0x15
#define CAN_PAY_CTRL_FIELD_COUNT            0x16  // Number of fields


// CAN message status bytes
#define CAN_STATUS_OK                   0x00
#define CAN_STATUS_INVALID_OPCODE       0x11
#define CAN_STATUS_INVALID_FIELD_NUM    0x12
#define CAN_STATUS_INVALID_DATA         0x13

#endif
