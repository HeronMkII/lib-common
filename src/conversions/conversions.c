/*
Conversions between raw data (bits represented as `uint` types) and actual
measurements for all software systems in the satellite.

ADC - ADS7952 - PAY/EPS
http://www.ti.com/lit/ds/slas605c/slas605c.pdf

EPS ADC uses current monitor - INA214
http://www.ti.com/lit/ds/symlink/ina214.pdf

Temperature sensor - LM95071 - PAY
http://www.ti.com/lit/ds/symlink/lm95071.pdf

Humidity sensor - HIH7131 - PAY
https://sensing.honeywell.com/honeywell-sensing-humidicon-hih7000-series-product-sheet-009074-6-en.pdf

Pressure sensor - MS5803-05BA - PAY
http://www.te.com/commerce/DocumentDelivery/DDEController?Action=showdoc&DocId=Data+Sheet%7FMS5803-05BA%7FB3%7Fpdf%7FEnglish%7FENG_DS_MS5803-05BA_B3.pdf%7FCAT-BLPS0011

Optical ADC - AD7194 - PAY-Optical, different from main ADC
http://www.analog.com/media/en/technical-documentation/data-sheets/AD7194.pdf
*/

#include <conversions/conversions.h>


/* ADC (common) */

/*
Converts raw 12 bit data from an ADC channel to the voltage (in V) on that ADC
input pin.
*/
double adc_raw_data_to_raw_voltage(uint16_t raw_data) {
    double ratio = (double) raw_data / (double) 0x0FFF;
    double voltage = ratio * ADC_V_REF;
    return voltage;
}


/* ADC (EPS) */

/*
Converts a raw voltage (read from an ADC channel pin) to a voltage in the
circuit using the known voltage divider.
*/
double adc_eps_raw_voltage_to_voltage(double raw_voltage) {
    // Use voltage divider circuit ratio to recover original voltage before division
    return raw_voltage / ADC_EPS_VOUT_DIV_RATIO;
}

/*
Converts a raw voltage (read from an ADC channel pin) to a current in the
circuit using the known current monitoring circuit.
*/
double adc_eps_raw_voltage_to_current(double raw_voltage) {
    double before_ref_voltage = raw_voltage - ADC_EPS_IOUT_VREF;

    // Get the voltage across the resistor before amplifier gain
    double before_gain_voltage = before_ref_voltage / ADC_EPS_IOUT_AMP_GAIN;
    // Ohm's law (I = V / R)
    double current = before_gain_voltage / ADC_EPS_IOUT_RES;

    return current;
}

/*
Converts raw 12 bit data from an ADC channel to a voltage in the circuit.
*/
double adc_eps_raw_data_to_voltage(uint16_t raw_data) {
    return adc_eps_raw_voltage_to_voltage(adc_raw_data_to_raw_voltage(raw_data));
}

/*
Converts raw 12 bit data from an ADC channel to a current in the circuit.
*/
double adc_eps_raw_data_to_current(uint16_t raw_data) {
    return adc_eps_raw_voltage_to_current(adc_raw_data_to_raw_voltage(raw_data));
}




/* Payload Environmental Sensors */

/*
Converts 16 bit raw data (INCLUDING the 0b11 on the right that is always there)
to temperature in degrees C (p. 9).
*/
double temp_raw_data_to_temperature(uint16_t raw_data) {
    int16_t signed_temp_data = ((int16_t) raw_data) / 4;
    // LSB is 0.03125 C
    return ((double) signed_temp_data) * 0.03125;
}

/*
Converts 14 bit raw data to a humidity measurement in %RH (relative humidity).
p.6
*/
double hum_raw_data_to_humidity(uint16_t raw_data) {
    return ((double) raw_data) / ((1 << 14) - 2.0) * 100.0;
}

/*
Converts 24 bit raw pressure data to the pressure in kPa.
Raw data - 0-6000 mbar with 0.01mbar resolution per bit
Datasheet says 0.03mbar resolution, but should be 0.01mbar

1 bar = 100,000 Pa
1 mbar = 100 Pa
1 kPa = 10 mbar
*/
double pres_raw_data_to_pressure(uint32_t raw_data) {
    double mbar = ((double) raw_data) * 0.01;
    double kpa = mbar / 10.0;
    return kpa;
}




/* Optical ADC */

/*
Converts a raw 24 bit measurement to the input voltage on the ADC pin (in V),
including applying the gain factor.
Unipolar operation (only positive)
*/
double optical_adc_raw_data_to_voltage(uint32_t raw_data, uint8_t gain) {
    // p.31
    // Code = (2^N * AIN * Gain) / (V_REF)
    // => AIN = (Code * V_REF) / (2^N * Gain)
    double num = ((double) raw_data) * ((double) OPTICAL_ADC_V_REF);
    double denom = (1UL << OPTICAL_ADC_N) * ((double) gain);
    return num / denom;
}
