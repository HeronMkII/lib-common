/*
Test the data conversion functions in the conversions library.
*/

#include <conversions/conversions.h>
#include <uart/uart.h>

void test_adc(void) {
    print("\nADC:\n");
    print("adc_raw_data_to_raw_vol: 0x3F7 -> %.3f V\n", adc_raw_data_to_raw_vol(0x3F7));
    print("adc_raw_data_to_raw_vol: 0x9E1 -> %.3f V\n", adc_raw_data_to_raw_vol(0x9E1));
    print("adc_eps_cur_to_raw_data: 0.95 A -> 0x%.3X\n", adc_eps_cur_to_raw_data(0.95));
    print("adc_raw_data_to_bat_cur: 0x3F7 -> %.3f A\n", adc_raw_data_to_bat_cur(0x3F7));
    print("adc_raw_data_to_bat_cur: 0x9E1 -> %.3f A\n", adc_raw_data_to_bat_cur(0x9E1));
}

// DAC conversions
void test_dac(void) {
    print("\nDAC:\n");
    print("dac_raw_data_to_vol: 0x3F7 -> %.3f V\n", dac_raw_data_to_vol(0x3F7));
    print("dac_vol_to_raw_data: 2.1 V -> 0x%.3X\n", dac_vol_to_raw_data(2.1));
}

void test_heaters(void) {
    print("\nHeaters:\n");
    print("dac_raw_data_to_heater_setpoint: 0x4AE -> %.3f C\n",
        dac_raw_data_to_heater_setpoint(0x4AE));
    print("heater_setpoint_to_dac_raw_data: 27.679 C -> 0x%.3X\n",
        heater_setpoint_to_dac_raw_data(27.679));
}

// thermistor conversions
void test_therm(void) {
    print("\nThermistors:\n");

    print("therm_res_to_temp: 18 k -> %.3f C\n", therm_res_to_temp(18.0));
    print("therm_temp_to_res: 31.2 C -> %.3f k\n", therm_temp_to_res(31.2));
    print("therm_res_to_vol: 112.9 k -> %.3f V\n", therm_res_to_vol(112.9));
    print("therm_vol_to_res: 2.3 V -> %.3f k\n", therm_vol_to_res(2.3));
}

void test_imu(void) {
    print("\nIMU:\n");

    print("imu_raw_data_to_gyro: 0x943A -> %.3f rad/s\n", imu_raw_data_to_gyro(0x943A));
    print("imu_raw_data_to_gyro: 0x21F0 -> %.3f rad/s\n", imu_raw_data_to_gyro(0x21F0));
}


int main(void) {
    init_uart();

    print("\n\nStarting test\n");
    test_adc();
    test_dac();
    test_heaters();
    test_therm();
    test_imu();
    print("\nDone test\n\n");

    while (1) {}
}
