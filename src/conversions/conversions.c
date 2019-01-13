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




/*
DAC
DAC7562
Datasheet: http://www.ti.com/lit/ds/symlink/dac8162.pdf
*/

/*
Converts raw data (12 bits) to an output voltage.
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
Converts an output voltage value to the raw data (12 bit) value.
voltage - output voltage (in V)
returns - 12 bit raw data
*/
uint16_t dac_vol_to_raw_data(double voltage) {
    // p.28 - 8.3.1
    // Vout = (Din / 2^n) x Vref x Gain
    // Din = (Vout x 2^n) / (Vref x Gain)
    double num = voltage * (1 << DAC_NUM_BITS);
    double denom = DAC_VREF * DAC_VREF_GAIN;
    uint16_t result = (uint16_t) (num / denom);

    return result;
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




/*
Thermistor conversions

For NTC Thermistor 10k 0603 (1608 Metric) Part # NCU18XH103F60RB
Digikey link: https://www.digikey.ca/product-detail/en/murata-electronics-north-america/NCU18XH103F60RB/490-16279-1-ND/7363262
Datasheet (page 13. Part # NCU__XH103):
https://www.murata.com/~/media/webrenewal/support/library/catalog/products/thermistor/r03e.ashx?la=en-us
Datasheet (NCU18XH103F60RB): https://www.murata.com/en-us/api/pdfdownloadapi?cate=luNTCforTempeSenso&partno=NCU18XH103F60RB

TODO - create test to verify PROGMEM values
TODO - confirm thermistor part number
*/

// Lookup table from manufacturer datasheet (pg 13)
// Resistances are stored in kilo-ohms
// PROGMEM instructs the compiler to store these values in flash memory
const float THERM_RES[THERM_LUT_COUNT] PROGMEM = {
    195.652,    148.171,    113.347,    87.559,     68.237,
    53.650,     42.506,     33.892,     27.219,     22.021,
    17.926,     14.674,     12.081,     10.000,     8.315,
    6.948,      5.834,      4.917,      4.161,      3.535,
    3.014,      2.586,      2.228,      1.925,      1.669,
    1.452,      1.268,      1.110,      0.974,      0.858,
    0.758,      0.672,      0.596,      0.531
};

// Temperatures in C
// TODO - should it be int8_t?
const int16_t THERM_TEMP[THERM_LUT_COUNT] PROGMEM = {
    -40,        -35,        -30,        -25,        -20,
    -15,        -10,        -5,         0,          5,
    10,         15,         20,         25,         30,
    35,         40,         45,         50,         55,
    60,         65,         70,         75,         80,
    85,         90,         95,         100,        105,
    110,        115,        120,        125
};


/*
Convert the measured thermistor resistance to temperature
resistance - thermistor resistance (in kilo-ohms)
Returns - temperature (in C)
*/
double therm_res_to_temp(double resistance){
    for (uint8_t i = 0; i < THERM_LUT_COUNT - 1; i++){
        // Next value should be smaller than previous value
        double resistance_next = pgm_read_float(&THERM_RES[i + 1]);

        if (resistance >= resistance_next){
            double resistance_prev = pgm_read_float(&THERM_RES[i]);
            int16_t temp_next = pgm_read_word(&THERM_TEMP[i + 1]);
            int16_t temp_prev = pgm_read_word(&THERM_TEMP[i]);

            double temp_diff = (double) (temp_next - temp_prev);
            double resistance_diff = (double) (resistance_next - resistance_prev);
            double slope = temp_diff / resistance_diff;

            double diff = resistance - resistance_prev;  //should be negative
            return temp_prev + (diff * slope);
        }
    }

    // TODO - this shouldn't happen
    return 0.0;
}

/*
Convert the thermistor temperature to resistance
temp - temperature (in C)
Returns - thermistor resistance (in kilo-ohms)
*/
double therm_temp_to_res(double temp) {
    for (uint8_t i = 0; i < THERM_LUT_COUNT - 1; i++){
        // Next value should be bigger than previous value
        int16_t temp_next = pgm_read_word(&THERM_TEMP[i + 1]);

        if (temp <= temp_next){
            int16_t temp_prev = pgm_read_word(&THERM_TEMP[i]);
            double resistance_next = pgm_read_float(&THERM_RES[i + 1]);
            double resistance_prev = pgm_read_float(&THERM_RES[i]);

            double resistance_diff = (double) (resistance_next - resistance_prev);
            double temp_diff = (double) (temp_next - temp_prev);
            double slope = resistance_diff / temp_diff;

            double diff = temp - temp_prev;  //should be positive
            return resistance_prev + (diff * slope);
        }
    }

    // TODO - this shouldn't happen
    return 0.0;
}

// Using the thermistor resistance, get the voltage at the point between the thermistor and the constant 10k resistor
// (10k connected to ground)
// See: https://www.allaboutcircuits.com/projects/measuring-temperature-with-an-ntc-thermistor/
double therm_res_to_vol(double resistance) {
    return THERM_V_REF * THERM_R_REF / (resistance + THERM_R_REF);
}

// Get the resistance of the thermistor given the voltage
// For equation, see: https://www.allaboutcircuits.com/projects/measuring-temperature-with-an-ntc-thermistor/
double therm_vol_to_res(double voltage) {
    return THERM_R_REF * (THERM_V_REF / voltage - 1);
}
