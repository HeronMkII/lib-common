#include <test/test.h>
#include <conversions/conversions.h>

void adc_data_to_voltage_test(void) {
    uint16_t raw_data_0 = 0x0FFF;
    double voltage_0 = 5.0; // 5V range
    ASSERT_FP_EQ(adc_raw_to_ch_vol(raw_data_0), voltage_0);

    uint16_t raw_data_1 = 0x0333;
    double voltage_1 = 1.0;
    ASSERT_FP_EQ(adc_raw_to_ch_vol(raw_data_1), voltage_1);
}

void adc_raw_vol_to_eps_vol_test(void) {
    double raw_voltage = 2.5;
    ASSERT_FP_EQ(adc_ch_vol_to_circ_vol(raw_voltage, 1e4, 1e4), 5.0);
}

void adc_raw_vol_to_eps_cur_test(void) {
    double raw_voltage = 2.64;
    double current = adc_ch_vol_to_circ_cur(raw_voltage, 0.008, 0.0);
    ASSERT_FP_EQ(current, 3.3);
}

void adc_raw_data_to_eps_vol_test(void) {
    uint16_t raw_data = 0x0FFF;
    ASSERT_FP_EQ(adc_raw_to_circ_vol(raw_data, 1e4, 1e4), 10.0);
}

void adc_raw_data_to_eps_cur_test(void) {
    uint16_t raw_data = 0x570;
    ASSERT_FP_EQ(adc_raw_to_circ_cur(raw_data, 0.01, 0.0), 1.7);
}

void adc_raw_to_efuse_cur_test(void) {
    ASSERT_FP_EQ(adc_raw_to_efuse_cur(0x570, 511), 13.521);
}

void adc_raw_data_to_therm_temp_test(void) {
    ASSERT_FP_EQ(adc_raw_to_therm_temp(0x1BA), -5.458);
}

void dac_raw_data_to_vol_test(void) {
    uint16_t raw_data = 0x3000;
    ASSERT_FP_EQ(dac_raw_data_to_vol(raw_data), 15.0);
}

void dac_vol_to_raw_data_test(void) {
    ASSERT_FP_EQ(dac_vol_to_raw_data(15.0), 0x3000);
}

void hum_raw_data_to_humidity_test(void) {
    uint32_t raw_data = (1UL << 13) - 1;
    ASSERT_FP_EQ(hum_raw_data_to_humidity(raw_data), 50.0);
}

void pres_raw_data_to_pressure_test(void) {
    uint32_t raw_data = (1UL << 16);
    ASSERT_FP_EQ(pres_raw_data_to_pressure(raw_data), 65.536);
}

void opt_raw_to_light_intensity_test(void) {
    // High gain (400x), 400ms integration
    uint32_t raw = (0b10UL << 22) | (0b011UL << 16) | (0xE7F9UL);
    ASSERT_FP_EQ(opt_raw_to_light_intensity(raw), 0.371);
}

void opt_power_test(void) {
    double voltage = 0.0;
    double current = 0.0;
    double power = 0.0;
    opt_power_raw_to_conv(0x2BC1EF, &voltage, &current, &power);
    ASSERT_FP_EQ(voltage, 2.256);
    ASSERT_FP_EQ(current, 1.595);
    ASSERT_FP_EQ(power, 3.599);
}

void therm_res_to_temp_test(void) {
    ASSERT_FP_EQ(therm_res_to_temp(200.0),      -37.319);
    ASSERT_FP_EQ(therm_res_to_temp(195.653),    -36.957);
    ASSERT_FP_EQ(therm_res_to_temp(195.652),    -36.957);
    ASSERT_FP_EQ(therm_res_to_temp(195.4),      -36.936);
    ASSERT_FP_EQ(therm_res_to_temp(148.171),    -32.278);
    ASSERT_FP_EQ(therm_res_to_temp(148.0),      -32.258);
    ASSERT_FP_EQ(therm_res_to_temp(10.0),       +25.0);
    ASSERT_FP_EQ(therm_res_to_temp(8.315),      +29.933);
    ASSERT_FP_EQ(therm_res_to_temp(7.29),       +33.551);
    ASSERT_FP_EQ(therm_res_to_temp(6.948),      +34.894);
    ASSERT_FP_EQ(therm_res_to_temp(0.54),       +128.38);
    ASSERT_FP_EQ(therm_res_to_temp(0.531),      +129.183);
    ASSERT_FP_EQ(therm_res_to_temp(0.52),       +130.188);
    ASSERT_FP_EQ(therm_res_to_temp(0.2),        +182.096);
}

void therm_temp_to_res_test(void) {
    ASSERT_FP_EQ(199.998,   therm_temp_to_res(-37.319));
    ASSERT_FP_EQ(195.653,   therm_temp_to_res(-36.957));
    ASSERT_FP_EQ(195.404,   therm_temp_to_res(-36.936));
    ASSERT_FP_EQ(148.17,    therm_temp_to_res(-32.278));
    ASSERT_FP_EQ(147.997,   therm_temp_to_res(-32.258));
    ASSERT_FP_EQ(10.0,      therm_temp_to_res(+25.0));
    ASSERT_FP_EQ(8.315,     therm_temp_to_res(+29.933));
    ASSERT_FP_EQ(7.29,      therm_temp_to_res(+33.551));
    ASSERT_FP_EQ(6.948,     therm_temp_to_res(+34.894));
    ASSERT_FP_EQ(0.54,      therm_temp_to_res(+128.38));
    ASSERT_FP_EQ(0.531,     therm_temp_to_res(+129.183));
    ASSERT_FP_EQ(0.52,      therm_temp_to_res(+130.188));
    ASSERT_FP_EQ(0.197,     therm_temp_to_res(+183.096));
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
test_t t3 = { .name = "adc_eps_raw_data_to_cur", .fn = adc_raw_vol_to_eps_cur_test };
test_t t4 = { .name = "adc_raw_data_to_eps_vol", .fn = adc_raw_data_to_eps_vol_test };
test_t t5 = { .name = "adc_eps_raw_data_to_cur", .fn = adc_raw_data_to_eps_cur_test };
test_t t6 = { .name = "adc_raw_to_efuse_cur", .fn = adc_raw_to_efuse_cur_test };
test_t t7 = { .name = "adc_raw_data_to_therm_temp", .fn = adc_raw_data_to_therm_temp_test };
test_t t8 = { .name = "dac_raw_data_to_vol", .fn = dac_raw_data_to_vol_test };
test_t t9 = { .name = "dac_vol_to_raw_data", .fn = dac_vol_to_raw_data_test };
test_t t10 = { .name = "hum_raw_data_to_humidity", .fn = hum_raw_data_to_humidity_test };
test_t t11 = { .name = "pres_raw_data_to_pressure", .fn = pres_raw_data_to_pressure_test };
test_t t12 = { .name = "opt_raw_to_light_intensity", .fn = opt_raw_to_light_intensity_test };
test_t t13 = { .name = "opt_power", .fn = opt_power_test };
test_t t14 = { .name = "therm_res_to_temp", .fn = therm_res_to_temp_test };
test_t t15 = { .name = "therm_temp_to_res", .fn = therm_temp_to_res_test };
test_t t16 = { .name = "therm_res_to_vol", .fn = therm_res_to_vol_test };
test_t t17 = { .name = "therm_vol_to_res", .fn = therm_vol_to_res_test };

test_t* suite[] = { &t1, &t2, &t3, &t4, &t5, &t6, &t7, &t8, &t9, &t10, &t11, &t12, &t13, &t14, &t15, &t16, &t17 };

int main() {
    run_tests(suite, sizeof(suite) / sizeof(suite[0]));
    return 0;
}
