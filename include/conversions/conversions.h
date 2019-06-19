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

#define ADC_EPS_BAT_IOUT_RES    0.002   // 2 mohm resistor
#define ADC_EPS_BAT_IOUT_VREF   2.5     // 2.5V REF (bidirectional)

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


// IMU Q points
#define IMU_ACCEL_Q 8
#define IMU_GYRO_Q  9


double adc_raw_data_to_raw_vol(uint16_t raw_data);
uint16_t adc_raw_vol_to_raw_data(double raw_voltage);
double adc_raw_vol_to_eps_vol(double raw_voltage);
double adc_raw_vol_to_eps_cur(double raw_voltage);
double adc_eps_cur_to_raw_vol(double current);
double adc_raw_vol_to_bat_cur(double raw_voltage);
double adc_raw_data_to_eps_vol(uint16_t raw_data);
double adc_raw_data_to_eps_cur(uint16_t raw_data);
uint16_t adc_eps_cur_to_raw_data(double current);
double adc_raw_data_to_bat_cur(uint16_t raw_data);
double adc_raw_data_to_therm_temp(uint16_t raw_data);

double dac_raw_data_to_vol(uint16_t raw_data);
uint16_t dac_vol_to_raw_data(double voltage);

double dac_raw_data_to_heater_setpoint(uint16_t raw_data);
uint16_t heater_setpoint_to_dac_raw_data(double temp);

double temp_raw_data_to_temperature(uint16_t raw_data);
double hum_raw_data_to_humidity(uint16_t raw_data);
double pres_raw_data_to_pressure(uint32_t raw_data);

double opt_adc_raw_data_to_vol(uint32_t raw_data, uint8_t gain);
double opt_adc_raw_data_to_diff_vol(uint32_t raw_data, uint8_t gain);

double therm_res_to_temp(double resistance);
double therm_temp_to_res(double temp);
double therm_res_to_vol(double resistance);
double therm_vol_to_res(double voltage);

double imu_raw_data_to_double(uint16_t raw_data, uint8_t q_point);
double imu_raw_data_to_gyro(uint16_t raw_data);

#endif
