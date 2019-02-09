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


#define OPT_ADC_V_REF       2.5 // reference voltage (in V)
#define OPT_ADC_NUM_BITS    24  // number of bits in raw data (called 'N' in datasheet)


// Reference (maximum) voltage for thermistor voltage divider
#define THERM_V_REF 2.5
// Reference resistance (connected to ground) in kohm
#define THERM_R_REF 10.0

// Lookup tables for thermistor data conversions
extern const float THERM_RES[];
extern const int16_t THERM_TEMP[];
// Number of points in arrays
#define THERM_LUT_COUNT 34


double adc_raw_data_to_raw_vol(uint16_t raw_data);
double adc_raw_vol_to_eps_vol(double raw_voltage);
double adc_raw_vol_to_eps_cur(double raw_voltage);
double adc_raw_data_to_eps_vol(uint16_t raw_data);
double adc_raw_data_to_eps_cur(uint16_t raw_data);
double adc_raw_data_to_therm_temp(uint16_t raw_data);

double dac_raw_data_to_vol(uint16_t raw_data);
uint16_t dac_vol_to_raw_data(double voltage);

double temp_raw_data_to_temperature(uint16_t raw_data);
double hum_raw_data_to_humidity(uint16_t raw_data);
double pres_raw_data_to_pressure(uint32_t raw_data);

double opt_adc_raw_data_to_vol(uint32_t raw_data, uint8_t gain);

double therm_res_to_temp(double resistance);
double therm_temp_to_res(double temp);
double therm_res_to_vol(double resistance);
double therm_vol_to_res(double voltage);

#endif
