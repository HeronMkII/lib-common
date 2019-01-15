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


// Internal voltage reference (V)
#define DAC_VREF        2.5
// Internal voltage reference gain
#define DAC_VREF_GAIN   2
// Number of bits to represent voltage (also referred to as 'N')
#define DAC_NUM_BITS    12


#define OPTICAL_ADC_V_REF   2.5 // reference voltage (in V)
#define OPTICAL_ADC_N       24  // number of bits in raw data


#define THERM_V_REF 2.5
// when referencing table in thermistors.c, values are in kilo Ohmns
// Reference in kohm
#define THERM_R_REF 10.0

extern const float THERM_RES[];
extern const int16_t THERM_TEMP[];
#define THERM_LUT_COUNT 34


double adc_raw_data_to_raw_voltage(uint16_t raw_data);
double adc_eps_raw_voltage_to_voltage(double raw_voltage);
double adc_eps_raw_voltage_to_current(double raw_voltage);
double adc_eps_raw_data_to_voltage(uint16_t raw_data);
double adc_eps_raw_data_to_current(uint16_t raw_data);

double dac_raw_data_to_vol(uint16_t raw_data);
uint16_t dac_vol_to_raw_data(double voltage);

double temp_raw_data_to_temperature(uint16_t raw_data);
double hum_raw_data_to_humidity(uint16_t raw_data);
double pres_raw_data_to_pressure(uint32_t raw_data);

double optical_adc_raw_data_to_voltage(uint32_t raw_data, uint8_t gain);

double therm_res_to_temp(double resistance);
double therm_temp_to_res(double temp);
double therm_res_to_vol(double resistance);
double therm_vol_to_res(double voltage);

#endif
