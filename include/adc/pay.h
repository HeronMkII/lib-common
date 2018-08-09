#ifndef ADC_PAY_H
#define ADC_PAY_H

// PAY ADC pinout
#define ADC_CS_PIN_PAY  PB6
#define ADC_CS_PORT_PAY PORTB
#define ADC_CS_DDR_PAY  DDRB

// PAY ADC channels - for thermistors
#define ADC_PAY_THM1_CH         0
#define ADC_PAY_THM2_CH         1
#define ADC_PAY_THM3_CH         2
#define ADC_PAY_THM4_CH         3
#define ADC_PAY_THM5_CH         4
#define ADC_PAY_THM6_CH         5
#define ADC_PAY_THM7_CH         7   // This is correct according to the schematic
#define ADC_PAY_THM8_CH         6   // (one pair out of order) - TODO verify
#define ADC_PAY_THM9_CH         8
#define ADC_PAY_THM10_CH        9
#define ADC_PAY_POS_PHT_1_CH    10
#define ADC_PAY_POS_PHT_2_CH    11

#endif // ADC_PAY_H
