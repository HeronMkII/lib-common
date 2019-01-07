/*
Test the data conversion functions in the conversions library.
*/

#include <conversions/conversions.h>
#include <uart/uart.h>

// thermistor conversions
void test_therm() {
    print("Thermistors:\n");

    print("therm_res_to_temp: 18k -> %.3f C\n", therm_res_to_temp(18.0));
    print("therm_temp_to_res: 31.2 C -> %.3fk\n", therm_temp_to_res(31.2));
    print("therm_res_to_vol: 112.9k -> %.3f V\n", therm_res_to_vol(112.9));
    print("therm_vol_to_res: 2.3 V -> %.3fk\n", therm_vol_to_res(2.3));
}


int main(void) {
    init_uart();

    print("\n\nStarting test\n\n");
    test_therm();
    print("\nDone test\n");

    while (1) {}
}
