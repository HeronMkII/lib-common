#include <test/test.h>
#include <conversions/conversions.h>

void adc_data_to_voltage_test(void) {
    uint16_t raw_data_0 = 0x0FFF;
    double voltage_0 = 5.0; // 5V range
    ASSERT_FP_EQ(adc_raw_data_to_raw_vol(raw_data_0), voltage_0);

    uint16_t raw_data_1 = 0x0333;
    double voltage_1 = 1.0;
    ASSERT_FP_EQ(adc_raw_data_to_raw_vol(raw_data_1), voltage_1);
}

void adc_raw_vol_to_eps_vol_test(void) {
    double raw_voltage = 2.5;
    ASSERT_FP_EQ(adc_raw_vol_to_eps_vol(raw_voltage), 5.0);
}

void adc_raw_vol_to_eps_cur_test(void) {
    double raw_voltage = 5.3;
    double current = adc_raw_vol_to_eps_cur(raw_voltage);
    ASSERT_FP_EQ(current, 2.0);
}

void adc_raw_data_to_eps_vol_test(void) {
    uint16_t raw_data = 0x0FFF;
    ASSERT_FP_EQ(adc_raw_data_to_eps_vol(raw_data), 10.0);
}

void adc_raw_data_to_eps_cur_test(void) {
    uint16_t raw_data = 0x0FFF;
    ASSERT_FP_EQ(adc_raw_data_to_eps_cur(raw_data), 1.7);
}

void adc_raw_data_to_therm_temp_test(void) {
    // 0x1BA -> 0.540 V -> 36.296 kohm -> -6.395 C (rounding goes to -6.411)
    ASSERT_FP_EQ(adc_raw_data_to_therm_temp(0x1BA), -6.411);
}

void dac_raw_data_to_vol_test(void) {
    uint16_t raw_data = 0x3000;
    ASSERT_FP_EQ(dac_raw_data_to_vol(raw_data), 15.0);
}

void dac_vol_to_raw_data_test(void) {
    ASSERT_FP_EQ(dac_vol_to_raw_data(15.0), 0x3000);
}

void temp_raw_data_to_temperature_test(void) {
    ASSERT_FP_EQ(temp_raw_data_to_temperature(129), 1);
}

void hum_raw_data_to_humidity_test(void) {
    uint32_t raw_data = (1UL << 13) - 1;
    ASSERT_FP_EQ(hum_raw_data_to_humidity(raw_data), 50.0);
}

void pres_raw_data_to_pressure_test(void) {
    uint32_t raw_data = (1UL << 16);
    ASSERT_FP_EQ(pres_raw_data_to_pressure(raw_data), 65.536);
}

void opt_adc_raw_data_to_vol_test(void) {
    uint32_t raw_data = 1UL << 23;
    uint8_t gain = 1 << 2;
    ASSERT_FP_EQ(opt_adc_raw_data_to_vol(raw_data, gain), 0.3125);
}

void therm_res_to_temp_test(void) {
    double resistance = 120.0;
    double temp = (double)(-35.0 + 5*(28.171/34.824)); //linear approximation
    ASSERT_FP_EQ(therm_res_to_temp(resistance), temp);
}

void therm_temp_to_res_test(void) {
    double resistance = 120.0;
    double temp = (double)(-35.0 + 5*(28.171/34.824));
    ASSERT_FP_EQ(therm_temp_to_res(temp), resistance);
}

void therm_res_to_vol_test(void) {
    double resistance = 10.0;
    double voltage = 1.25;
    ASSERT_FP_EQ(therm_res_to_vol(resistance), voltage);
}

void therm_vol_to_res_test(void) {
    double voltage = 1.25;
    double therm_res = 10.0;
    ASSERT_FP_EQ(therm_vol_to_res(voltage), therm_res);
}

test_t t1 = { .name = "adc_data_to_voltage", .fn = adc_data_to_voltage_test };
test_t t2 = { .name = "adc_raw_vol_to_eps_vol", .fn = adc_raw_vol_to_eps_vol_test };
test_t t3 = { .name = "adc_eps_raw_data_to_curret", .fn = adc_raw_vol_to_eps_cur_test };
test_t t4 = { .name = "adc_raw_data_to_eps_vol", .fn = adc_raw_data_to_eps_vol_test };
test_t t5 = { .name = "adc_eps_raw_data_to_curret", .fn = adc_raw_data_to_eps_cur_test };
test_t t6 = { .name = "adc_raw_data_to_therm_temp", .fn = adc_raw_data_to_therm_temp_test };
test_t t7 = { .name = "dac_raw_data_to_vol", .fn = dac_raw_data_to_vol_test };
test_t t8 = { .name = "dac_vol_to_raw_data", .fn = dac_vol_to_raw_data_test };
test_t t9 = { .name = "temp_raw_data_to_temperature", .fn = temp_raw_data_to_temperature_test };
test_t t10 = { .name = "hum_raw_data_to_humidity", .fn = hum_raw_data_to_humidity_test };
test_t t11 = { .name = "pres_raw_data_to_pressure", .fn = pres_raw_data_to_pressure_test };
test_t t12 = { .name = "opt_adc_raw_data_to_vol", .fn = opt_adc_raw_data_to_vol_test };
test_t t13 = { .name = "therm_res_to_temp", .fn = therm_res_to_temp_test };
test_t t14 = { .name = "therm_temp_to_res", .fn = therm_temp_to_res_test };
test_t t15 = { .name = "therm_res_to_vol", .fn = therm_res_to_vol_test };
test_t t16 = { .name = "therm_vol_to_res", .fn = therm_vol_to_res_test };

test_t* suite[16] = { &t1, &t2, &t3, &t4, &t5, &t6, &t7, &t8, &t9, &t10, &t11, &t12, &t13, &t14, &t15, &t16 };

int main() {
    run_tests(suite, 16);
    return 0;
}
