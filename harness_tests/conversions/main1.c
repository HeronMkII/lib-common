#include <test/test.h>
#include <conversions/conversions.h>

void adc_data_to_voltage_test(void) {
    uint16_t raw_data_0 = 0x0FFF;
    double voltage_0 = 2.5;
    ASSERT_EQ(adc_data_to_voltage(raw_data), voltage_0);

    uint16_t raw_data_1 = 0x0333; 
    double voltage_1 = 0.5;
    ASSERT_EQ(adc_data_to_voltage(raw_data_1), voltage_1);
}

void adc_eps_raw_voltage_to_voltage_test(void) {
    double raw_voltage = 2.5;
    ASSERT_EQ(adc_data_to_voltage(raw_voltage), 5.0);
}

void adc_eps_raw_voltage_to_current_test(void) {
    double raw_voltage = 5.3;
    ASSERT_EQ(adc_data_to_voltage_to_current(raw_voltage), 2.0);
}

void adc_eps_raw_data_to_voltage_test(void) {
    uint16_t raw_data = 0x0FFF;
    ASSERT_EQ(adc_eps_raw_data_to_voltage(raw_data), 10.0);
}

void adc_eps_raw_data_to_current_test(void) {
    uint16_t raw_data = 0x0FFF;
    ASSERT_EQ(adc_eps_raw_data_to_current(), 1.7);
}

void dac_raw_data_to_vol_test(void) {
    uint16_t raw_data = 0x3000;
    ASSERT_EQ(dac_raw_data_to_vol(raw_data), 15.0);
}

void dac_vol_to_raw_data_test(void) {
    ASSERT_EQ(dac_vol_to_raw_data(15.0), 0x3000);
}

void temp_raw_data_to_temperature_test(void) {
    ASSERT_EQ(temp_raw_data_to_temperature(129), 1);
}

void hum_raw_data_to_humidity_test(void) {
    uint32_t raw_data = (1 << 13) - 1; 
    ASSERT_EQ(raw_data_to_humidity(raw_data), 50.0);
}

void pres_raw_data_to_pressure_test(void) {
    uint32_t raw_data = (1 << 16);
    ASSERT_EQ(pres_raw_data_to_pressure(raw_data), 65.536);
}

void optical_adc_raw_data_to_voltage_test(void) {
    uint32_t raw_data = 1 << 23;
    uint8_t gain = 1 < 2;
    ASSERT_EQ(optical_adc_raw_data_to_voltage(raw_data, gain), 0.3125);
}

void therm_res_to_temp_test(void) {
    double resistance = 120.0;
    double temp = (double)(-35.0 + 5*(28.171/34.824)); //linear approximation
    ASSERT_EQ(therm_res_to_temp(120.0), temp);
}

void therm_temp_to_res_test(void) {
    double resistance = 120.0;
    double temp = (double)(-35.0 + 5*(28.171/34.824));
    ASSERT_EQ(therm_temp_to_res(temp), resistance);
}

void therm_res_to_vol_test(void) {
    double resistance = 10.0;
    double voltage = 1.25;
    ASSERT_EQ(therm_res_to_vol(resistance), 1.25);
}

void therm_vol_to_res_test(void) {
    double voltage = 1.25;
    double therm_res = 10.0;
    ASSERT_EQ(therm_vol_to_res(voltage), therm_res);
}

test_t t1 = { .name = "adc_data_to_voltage", .fn = adc_data_to_voltage_test };
test_t t2 = { .name = "adc_eps_raw_voltage_to_voltage", .fn =adc_eps_raw_voltage_to_voltage_test };
test_t t3 = { .name = "adc_eps_raw_data_to_curret", .fn = adc_eps_raw_voltage_to_current_test };
test_t t4 = { .name = "adc_eps_raw_data_to_voltage", .fn = adc_eps_raw_data_to_voltage_test };
test_t t5 = { .name = "adc_eps_raw_data_to_curret", .fn = adc_eps_raw_data_to_current_test };
test_t t6 = { .name = "dac_raw_data_to_vol", .fn = dac_raw_data_to_vol_test };
test_t t7 = { .name = "dac_vol_to_raw_data", .fn = dac_vol_to_raw_data_test };
test_t t8 = { .name = "temp_raw_data_to_temperature", .fn = temp_raw_data_to_temperature_test };
test_t t9 = { .name = "hum_raw_data_to_humidity", .fn = hum_raw_data_to_humidity_test };
test_t t10 = { .name = "pres_raw_data_to_pressure", .fn = pres_raw_data_to_pressure_test };
test_t t11 = { .name = "optical_adc_raw_data_to_voltage", .fn = optical_adc_raw_data_to_voltage_test };
test_t t12 = { .name = "therm_res_to_temp", .fn = therm_res_to_temp_test };
test_t t13 = { .name = "therm_temp_to_res", .fn = therm_temp_to_res_test };
test_t t14 = { .name = "therm_res_to_vol", .fn = therm_res_to_vol_test };
test_t t15 = { .name = "therm_vol_to_res", .fn = therm_vol_to_res_test };


test_t* suite[15] = { &t1, &t2, &t3, &t4, &t5, &t6, &t7, &t8, &t9, &t10, &t11, &t12, &t13, &t14, &t15 };

int main() {
    run_tests(suite, 15);
    return 0;
}