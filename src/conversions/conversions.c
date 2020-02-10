/*
Conversions library

All conversions are based on the data conversion protocol:
https://utat-ss.readthedocs.io/en/master/our-protocols/data-conversion.html

Conversion functions between raw data (bits represented as `uint` types) and
actual measurements for all software systems in the satellite.

ADC - ADS7952 - PAY/EPS
http://www.ti.com/lit/ds/slas605c/slas605c.pdf

EPS/PAY ADCs use current monitor - INA214
http://www.ti.com/lit/ds/symlink/ina214.pdf

EPS uses eFuse for battery current to PAY - TPS25982
http://www.ti.com/lit/ds/symlink/tps25982.pdf

DAC - DAC7562 - PAY
Datasheet: http://www.ti.com/lit/ds/symlink/dac8162.pdf

Humidity sensor - HIH7131 - PAY
https://sensing.honeywell.com/honeywell-sensing-humidicon-hih7000-series-product-sheet-009074-6-en.pdf

Pressure sensor - MS5803-05BA - PAY
http://www.te.com/commerce/DocumentDelivery/DDEController?Action=showdoc&DocId=Data+Sheet%7FMS5803-05BA%7FB3%7Fpdf%7FEnglish%7FENG_DS_MS5803-05BA_B3.pdf%7FCAT-BLPS0011

Optical Sensor - TSL2591
https://ams.com/documents/20143/36005/TSL2591_DS000338_6-00.pdf

Thermistor - ERT-J0EG103FA:
https://www.digikey.ca/product-detail/en/panasonic-electronic-components/ERT-J0EG103FA/P12007CT-ND/526624
https://industrial.panasonic.com/cdbs/www-data/pdf/AUA0000/AUA0000C8.pdf
https://www.murata.com/~/media/webrenewal/support/library/catalog/products/thermistor/r03e.ashx?la=en-us
http://www.resistorguide.com/ntc-thermistor/

IMU - BNO080
https://cdn.sparkfun.com/assets/1/3/4/5/9/BNO080_Datasheet_v1.3.pdf
https://cdn.sparkfun.com/assets/4/d/9/3/8/SH-2-Reference-Manual-v1.2.pdf
https://cdn.sparkfun.com/assets/7/6/9/3/c/Sensor-Hub-Transport-Protocol-v1.7.pdf
*/

#include <conversions/conversions.h>
#include <uart/uart.h>

// Uncomment for logging
// #define CONVERSIONS_DEBUG

/*
Converts raw data from an ADC channel to the voltage on that ADC channel input pin.
raw_data - 12 bit ADC data
returns - voltage on ADC input channel pin (in V)
*/
double adc_raw_to_ch_vol(uint16_t raw) {
    double ratio = (double) raw / (double) 0x0FFF;
    double voltage = ratio * ADC_VREF;
    return voltage;
}

/*
Converts the voltage on an ADC channel input pin to raw data from an ADC channel.
raw_data - voltage on ADC input channel pin (in V)
returns - 12 bit ADC data
*/
uint16_t adc_ch_vol_to_raw(double ch_vol) {
    return (uint16_t) ((ch_vol / (double) ADC_VREF) * 0x0FFF);
}

/*
Converts a voltage on an ADC channel pin to a voltage in the circuit using
    a voltage divider ratio.
raw_voltage - voltage on an ADC channel input pin (in V)
low_res - low-side resistance (in ohms)
high_res - high-side resistance (in ohms)
returns - voltage in the circuit (in V)
*/
double adc_ch_vol_to_circ_vol(double ch_vol, double low_res, double high_res) {
    // Use voltage divider circuit ratio to recover original voltage before division
    return ch_vol / low_res * (low_res + high_res);
}

/*
Converts a raw voltage on an ADC pin to a current in the circuit using
    the known sense resistor current monitoring circuit.
ch_vol - voltage on an ADC input pin (in V)
sense_res - sense resistor value (in ohms)
ref_vol - VREF (reference voltage) value to the INA214 (in V)
returns - current in the circuit (in A)
*/
double adc_ch_vol_to_circ_cur(double ch_vol, double sense_res, double ref_vol) {
    // Get the voltage across the sense resistor before amplifier gain
    double before_gain_voltage = (ch_vol - ref_vol) / ADC_CUR_SENSE_AMP_GAIN;
    // Ohm's law (I = V / R)
    double circ_cur = before_gain_voltage / sense_res;
    return circ_cur;
}

/*
Converts a raw voltage on an ADC pin to the output current of the eFuse.
ch_vol - voltage on an ADC input pin (in V)
sense_res - sense resistor value (in ohms)
returns - current in the circuit (in A)
*/
double adc_ch_vol_to_efuse_cur(double ch_vol, double sense_res) {
    double iout = ch_vol / (EFUSE_IMON_CUR_GAIN * sense_res);
    return iout;
}

/*
Converts a current in the circuit to a channel voltage on an ADC pin using
    the known sense resistor current monitoring circuit.
circ_cur - current in the circuit (in A)
sense_res - sense resistor value (in ohms)
ref_vol - VREF (reference voltage) value to the INA214 (in V)
returns - voltage on an ADC channel input pin (in V)
*/
double adc_circ_cur_to_ch_vol(double circ_cur, double sense_res, double ref_vol) {
    // Ohm's law (V = I * R)
    double before_gain_voltage = circ_cur * sense_res;
    // Get the voltage to the ADC input after amplifier gain (with reference offset)
    double ch_vol = (before_gain_voltage * ADC_CUR_SENSE_AMP_GAIN) + ref_vol;
    return ch_vol;
}

/*
Converts raw 12 bit data from an ADC channel to a voltage in the circuit.
raw - 12 bits
returns - in V
*/
double adc_raw_to_circ_vol(uint16_t raw, double low_res, double high_res) {
    return adc_ch_vol_to_circ_vol(adc_raw_to_ch_vol(raw), low_res, high_res);
}

/*
Converts raw 12 bit data from an ADC channel to a current in the circuit.
raw - 12 bits
returns - in A
*/
double adc_raw_to_circ_cur(uint16_t raw, double sense_res, double ref_vol) {
    return adc_ch_vol_to_circ_cur(adc_raw_to_ch_vol(raw), sense_res, ref_vol);
}

/*
Converts a current in the EPS circuit to raw 12 bit data from an ADC channel.
current - in A
returns - 12 bits
*/
uint16_t adc_circ_cur_to_raw(double circ_cur, double sense_res, double ref_vol) {
    return adc_ch_vol_to_raw(adc_circ_cur_to_ch_vol(circ_cur, sense_res, ref_vol));
}

/*
Converts a raw 12-bit ADC value to the output current of the eFuse.
raw - 12 bits
sense_res - sense resistor value (in ohms)
returns - current in the circuit (in A)
*/
double adc_raw_to_efuse_cur(uint16_t raw, double sense_res) {
    double iout = adc_ch_vol_to_efuse_cur(adc_raw_to_ch_vol(raw), sense_res);
    return iout;
}


/*
Converts raw 12 bit data from an ADC channel to the temperature measured by a
    thermistor.
raw_data - 12 bits
returns - in C
*/
double adc_raw_to_therm_temp(uint16_t raw) {
    return therm_res_to_temp(therm_vol_to_res(adc_raw_to_ch_vol(raw)));
}


/*
Converts DAC raw data to an output voltage.
raw_data - 12 bit raw data (Din)
returns - output voltage (in V)
*/
double dac_raw_data_to_vol(uint16_t raw_data) {
    // p.28 - 8.3.1
    // Vout = (Din / 2^n) x Vref x Gain
    double ratio = ((double) raw_data) / ((double) (1 << DAC_NUM_BITS));
    double result = ratio * DAC_VREF * DAC_VREF_GAIN;

    return result;
}

/*
Converts a DAC output voltage value to the raw data (12 bit) value.
voltage - output voltage (in V)
returns - 12 bit raw data
*/
uint16_t dac_vol_to_raw_data(double voltage) {
    // p.28 - 8.3.1
    // Vout = (Din / 2^n) x Vref x Gain
    // Din = (Vout x 2^n) / (Vref x Gain)
    double num = voltage * ((double) (1 << DAC_NUM_BITS));
    double denom = DAC_VREF * DAC_VREF_GAIN;
    uint16_t result = (uint16_t) (num / denom);

    return result;
}


double dac_raw_data_to_heater_setpoint(uint16_t raw_data) {
    double vol = dac_raw_data_to_vol(raw_data);
    double res = therm_vol_to_res(vol);
    double temp = therm_res_to_temp(res);
    return temp;
}

// temp - in C
// Returns - raw (12 bits)
uint16_t heater_setpoint_to_dac_raw_data(double temp) {
    double res = therm_temp_to_res(temp);
    double vol = therm_res_to_vol(res);
    uint16_t raw_data = dac_vol_to_raw_data(vol);
    return raw_data;
}


/*
Converts raw data to a humidity measurement (p.6).
raw_data - 14 bits
returns - humidity (in %RH, relative humidity)
*/
double hum_raw_data_to_humidity(uint16_t raw_data) {
    return ((double) raw_data) / ((1 << 14) - 2.0) * 100.0;
}

/*
Converts raw pressure data to the pressure.
raw_data - 24 bits, 0-6000 mbar with 0.01mbar resolution per bit
    datasheet says 0.03mbar resolution, but should be 0.01mbar
returns - pressure (in kPa)

1 bar = 100,000 Pa
1 mbar = 100 Pa
1 kPa = 10 mbar
*/
double pres_raw_data_to_pressure(uint32_t raw_data) {
    double mbar = ((double) raw_data) * 0.01;
    double kpa = mbar / 10.0;
    return kpa;
}


double opt_adc_raw_to_ch_vol(uint16_t raw) {
    return ((double) raw) / ((double) (1 << OPT_ADC_BITS)) * OPT_ADC_VREF;
}

/*
Converts raw 24 bits (2 measurements) to voltage, current, and power.
voltage - in V
current - in A
power - in W
*/
void opt_power_raw_to_conv(
        uint32_t raw, double* voltage, double* current, double* power) {
    uint16_t voltage_raw = (raw >> 12) & 0x3FF;
    uint16_t current_raw = raw & 0x3FF;

    *voltage = opt_adc_raw_to_ch_vol(voltage_raw);
    *current = (opt_adc_raw_to_ch_vol(current_raw) / OPT_ADC_CUR_SENSE_AMP_GAIN)
        / OPT_ADC_CUR_SENSE;
    *power = (*voltage) * (*current);
}


/*
Converts bits representing gain to actual gain.
p.8,16
Only using channel 0
*/
double opt_gain_raw_to_conv(uint8_t raw) {
    switch (raw) {
        // Low
        case 0b00:
            return 1.0;
        // Medium
        case 0b01:
            return 24.5;
        // High
        case 0b10:
            return 400.0;
        // Max
        case 0b11:
            return 9200.0;
        default:
            return 1.0;
    }
}

/*
Converts bits representing integration time to integration time (in ms).
p.16
*/
double opt_int_time_raw_to_conv(uint8_t raw) {
    return (((double) raw) + 1) * 100;
}

/*
Format:
bits[23:22] are gain
bits[18:16] are integration time
bits[15:0] are the data
Returns intensity value in ADC counts / ms
*/
double opt_raw_to_light_intensity(uint32_t raw) {
    double gain = opt_gain_raw_to_conv((raw >> 22) & 0x03);
    double int_time = opt_int_time_raw_to_conv((raw >> 16) & 0x07);
    double reading = (double) (raw & 0xFFFF);
    return reading / (gain * int_time);
}


/*
Converts the measured thermistor resistance to temperature.
resistance - thermistor resistance (in kilo-ohms)
Returns - temperature (in C)
*/
double therm_res_to_temp(double resistance){
    double denom = (log(resistance / THERM_NOM_RES) / THERM_BETA)
        + (1.0 / THERM_NOM_TEMP);
    return (1.0 / denom) - THERM_CELSIUS_TO_KELVIN;
}

/*
Converts the thermistor temperature to resistance.
temp - temperature (in C)
Returns - thermistor resistance (in kilo-ohms)
*/
double therm_temp_to_res(double temp) {
    double temp_diff = (1.0 / (temp + THERM_CELSIUS_TO_KELVIN))
        - (1.0 / THERM_NOM_TEMP);
    return THERM_NOM_RES * exp(THERM_BETA * temp_diff);
}

/*
Using the thermistor resistance, get the voltage at the point between the
    thermistor and the constant 10k resistor (10k connected to ground)
See: https://www.allaboutcircuits.com/projects/measuring-temperature-with-an-ntc-thermistor/
resistance - in kilo-ohms
returns - voltage (in V)
*/
double therm_res_to_vol(double resistance) {
    return THERM_V_REF * THERM_R_REF / (resistance + THERM_R_REF);
}

/*
Gets the resistance of the thermistor given the voltage.
For equation, see: https://www.allaboutcircuits.com/projects/measuring-temperature-with-an-ntc-thermistor/
voltage - in V
returns - resistance (in kilo-ohms)
*/
double therm_vol_to_res(double voltage) {
    return THERM_R_REF * (THERM_V_REF / voltage - 1);
}


/*
IMU Q-point
Converts the raw 16-bit signed fixed-point value from the input report to the actual floating-point measurement using the Q point.
Q point - number of fractional digits after (to the right of) the decimal point, i.e. higher Q point means smaller/more precise number (#1 p.22)
https://en.wikipedia.org/wiki/Q_(number_format)
Similar to reference library qToFloat()
raw_data - 16 bit raw value
q_point - number of binary digits to shift
*/
double imu_raw_data_to_double(uint16_t raw_data, uint8_t q_point) {
    // Implement power of 2 with a bitshift instead of pow(), which links to the
    // math library and increases the binary size by ~1.3kB
    int16_t raw_signed = (int16_t) raw_data;
    return ((double) raw_signed) / ((double) (1 << q_point));
}

/*
Converts the raw 16-bit value to a gyroscope measurement (in rad/s).
*/
double imu_raw_data_to_gyro(uint16_t raw_data) {
    return imu_raw_data_to_double(raw_data, IMU_GYRO_Q);
}
