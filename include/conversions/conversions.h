#ifndef CONVERSIONS_H
#define CONVERSIONS_H

#include <stdint.h>

#include <avr/pgmspace.h>


// Reference voltage (with doubler enabled)
#define ADC_VREF 5.0
// Current conversion for INA214
#define ADC_CUR_SENSE_AMP_GAIN   100.0   // 100x voltage gain


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
extern const float THERM_TEMP[];
// Number of points in arrays
#define THERM_LUT_COUNT 34


// IMU Q points
#define IMU_ACCEL_Q 8
#define IMU_GYRO_Q  9


double adc_raw_to_ch_vol(uint16_t raw);
uint16_t adc_ch_vol_to_raw(double ch_vol);
double adc_ch_vol_to_circ_vol(double ch_vol, double low_res, double high_res);
double adc_ch_vol_to_circ_cur(double ch_vol, double sense_res, double ref_vol);
double adc_circ_cur_to_ch_vol(double circ_cur, double sense_res, double ref_vol);
double adc_raw_to_circ_vol(uint16_t raw, double low_res, double high_res);
double adc_raw_to_circ_cur(uint16_t raw, double sense_res, double ref_vol);
uint16_t adc_circ_cur_to_raw(double circ_cur, double sense_res, double ref_vol);
double adc_raw_to_therm_temp(uint16_t raw);

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
