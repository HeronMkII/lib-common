#ifndef CONVERSIONS_H
#define CONVERSIONS_H

#include <stdint.h>

#include <avr/pgmspace.h>

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

#define OPTICAL_ADC_V_REF   2.5 // reference voltage (in V)
#define OPTICAL_ADC_N       24  // number of bits in raw data

#define THERMIS_V_REF     2.5
#define THERMIS_R_REF 10.0 // when referencing table in thermistors.c, values are in kilo Ohmns

// Resistances are stored in kilo-ohms
// PROGMEM instructs the compiler to store these values in flash memory
extern const float THERM_RES[];
extern const int THERM_TEMP[];
#define THERM_LUT_COUNT 34


double adc_raw_data_to_raw_voltage(uint16_t raw_data);
double adc_eps_raw_voltage_to_voltage(double raw_voltage);
double adc_eps_raw_voltage_to_current(double raw_voltage);
double adc_eps_raw_data_to_voltage(uint16_t raw_data);
double adc_eps_raw_data_to_current(uint16_t raw_data);

double temp_raw_data_to_temperature(uint16_t raw_data);
double hum_raw_data_to_humidity(uint16_t raw_data);
double pres_raw_data_to_pressure(uint32_t raw_data);

double optical_adc_raw_data_to_voltage(uint32_t raw_data, uint8_t gain);

double thermis_resistance_to_temp(double resistance);
double thermis_temp_to_resistance(double temp);
double thermis_resistance_to_voltage(double resistance);
double thermis_voltage_to_resistance(double voltage);

#endif
