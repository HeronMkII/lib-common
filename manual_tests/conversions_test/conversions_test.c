/*
Test the data conversion functions in the conversions library.
*/

#include <conversions/conversions.h>
#include <uart/uart.h>

// DAC conversions
void test_dac(void) {
    print("\nDAC:\n");
    print("dac_raw_data_to_vol: 0x3F7 -> %.3f V\n", dac_raw_data_to_vol(0x3F7));
    print("dac_vol_to_raw_data: 2.1 V -> 0x%.3X\n", dac_vol_to_raw_data(2.1));
}

// thermistor conversions
void test_therm(void) {
    print("\nThermistors:\n");

    print("therm_res_to_temp: 18 k -> %.3f C\n", therm_res_to_temp(18.0));
    print("therm_temp_to_res: 31.2 C -> %.3f k\n", therm_temp_to_res(31.2));
    print("therm_res_to_vol: 112.9 k -> %.3f V\n", therm_res_to_vol(112.9));
    print("therm_vol_to_res: 2.3 V -> %.3f k\n", therm_vol_to_res(2.3));
}


int main(void) {
    init_uart();

    print("\n\nStarting test\n");
    test_dac();
    test_therm();
    print("\nDone test\n\n");

    while (1) {}
}
