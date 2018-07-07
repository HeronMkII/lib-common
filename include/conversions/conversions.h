#ifndef CONVERSIONS_H
#define CONVERSIONS_H

#include <stdint.h>

// Reference voltage
#define ADC_V_REF 5.0

// Voltage conversion
// Voltage divider ratio using resistors
#define ADC_EPS_VOUT_DIV_RATIO  0.5     // equal resistors in voltage divider

// Current conversion
// Ammeter - INA214
#define ADC_EPS_IOUT_RES        0.010   // 10 mohm resistor
#define ADC_EPS_IOUT_AMP_GAIN   100.0   // 100x voltage gain
#define ADC_EPS_IOUT_VREF       3.3     // 3.3V reference

double adc_raw_data_to_raw_voltage(uint16_t raw_data);
double adc_eps_raw_voltage_to_voltage(double raw_voltage);
double adc_eps_raw_voltage_to_current(double raw_voltage);
double adc_eps_raw_data_to_voltage(uint16_t raw_data);
double adc_eps_raw_data_to_current(uint16_t raw_data);

#endif
