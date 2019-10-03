#include <test/test.h>
#include <utilities/utilities.h>

void eeprom_test(void) {
    // Test parameters that properly uses 16 bits of address and 32 bits of data
    uint16_t addr = 0x720;
    uint32_t default_value = 0x12345678;

    ASSERT_EQ(read_eeprom(addr, EEPROM_DEF_DWORD), EEPROM_DEF_DWORD);
    ASSERT_EQ(read_eeprom(addr, default_value), default_value);

    uint32_t write_data = 0x7A35FE09;
    
    ASSERT_NEQ(write_data, default_value);
    write_eeprom(addr, write_data);

    ASSERT_EQ(read_eeprom(addr, default_value), write_data);
}

test_t t1 = { .name = "EEPROM test", .fn = eeprom_test };

test_t* suite[] = { &t1 };

int main(void){
    run_tests(suite, sizeof(suite) / sizeof(suite[0]));
    return 0;
}
