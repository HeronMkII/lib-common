/*
Conversions between raw data (bits represented as `uint` types) and actual
measurements for all software systems in the satellite.
*/

#include <conversions/conversions.h>


/* ADC (common between PAY and EPS) */

// Converts raw 12 bit data from an ADC channel
// to the voltage (in V) on that ADC input pin
double adc_raw_data_to_raw_voltage(uint16_t raw_data) {
    double ratio = (double) raw_data / (double) 0x0FFF;
    double voltage = ratio * ADC_V_REF;
    return voltage;
}


/* ADC (specific to EPS) */

// Converts a raw voltage (read from an ADC channel pin)
// to a voltage in the circuit using the known voltage divider
double adc_eps_raw_voltage_to_voltage(double raw_voltage) {
    // Use voltage divider circuit ratio to recover original voltage before division
    return raw_voltage / ADC_EPS_VOUT_DIV_RATIO;
}

// Converts a raw voltage (read from an ADC channel pin)
// to a current in the circuit using the known current monitoring circuit
double adc_eps_raw_voltage_to_current(double raw_voltage) {
    double before_ref_voltage = raw_voltage - ADC_EPS_IOUT_VREF;

    // Get the voltage across the resistor before amplifier gain
    double before_gain_voltage = before_ref_voltage / ADC_EPS_IOUT_AMP_GAIN;
    // Ohm's law (I = V / R)
    double current = before_gain_voltage / ADC_EPS_IOUT_RES;

    return current;
}

// Converts raw 12 bit data from an ADC channel
// to a voltage in the circuit
double adc_eps_raw_data_to_voltage(uint16_t raw_data) {
    return adc_eps_raw_voltage_to_voltage(adc_raw_data_to_raw_voltage(raw_data));
}

// Converts raw 12 bit data from an ADC channel
// to a current in the circuit
double adc_eps_raw_data_to_current(uint16_t raw_data) {
    return adc_eps_raw_voltage_to_current(adc_raw_data_to_raw_voltage(raw_data));
}
