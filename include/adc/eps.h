#ifndef ADC_EPS_H
#define ADC_EPS_H

// EPS ADC pinout
#define ADC_CS_PIN_EPS  PB3
#define ADC_CS_PORT_EPS PORTB
#define ADC_CS_DDR_EPS  DDRB

// EPS ADC channels
// Buck-boost converters output voltage (BB1 and BB2 joined together) (BBV)
#define ADC_EPS_BB_VOUT_CH          0
// Boost converters output voltage (BT1 and BT2 joined together) (BTV)
#define ADC_EPS_BT_VOUT_CH          3
// Battery pack positive voltage (VPACK/+PACK)
#define ADC_EPS_BATT_VPOS_CH        10
// Battery pack negative voltage (-PACK)
#define ADC_EPS_BATT_VNEG_CH        5

// Buck-boost converter 1 output current (BB1I)
#define ADC_EPS_BB1_IOUT_CH         1
// Buck-boost converter 2 output current (BB2I)
#define ADC_EPS_BB2_IOUT_CH         2
// Boost converter 1 output current (BT1I)
#define ADC_EPS_BT1_IOUT_CH         4
// Battery output current (IPACK)
#define ADC_EPS_BATT_IOUT_CH        11

// Photovoltaic cell (+X) output current (+XI)
#define ADC_EPS_PV_POS_X_IOUT_CH    6
// Photovoltaic cell (-X) output current (-XI)
#define ADC_EPS_PV_NEG_X_IOUT_CH    7
// Photovoltaic cell (+Y) output current (+YI)
#define ADC_EPS_PV_POS_Y_IOUT_CH    8
// Photovoltaic cell (-Y) output current (-YI)
#define ADC_EPS_PV_NEG_Y_IOUT_CH    9

#endif // ADC_EPS_H
