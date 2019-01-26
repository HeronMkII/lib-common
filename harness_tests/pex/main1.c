#include <test/test.h>
#include <pex/pex.h>

/*
    The goals of this harness test are:
    1. Make sure pex initializes correctly
    2. We can write to entire registers
    3. We can read individual pins
    4. We can set individual pins
*/

pin_info_t cs = {
    .port = &PEX_CS_PORT_PAY,
    .ddr = &PEX_CS_DDR_PAY,
    .pin = PEX_CS_PIN_PAY
};

pin_info_t rst = {
    .port = &PEX_RST_PORT_PAY,
    .ddr = &PEX_RST_DDR_PAY,
    .pin = PEX_RST_PIN_PAY
};

pex_t pex = {
    .addr = PEX_ADDR_PAY,
    .cs = &cs,
    .rst = &rst
};

// Test that the pex has been correctly initialized
void init_pex_test(void) {
    init_pex(&pex);

    ASSERT_EQ(read_register(&pex, PEX_IOCON), PEX_IOCON_DEFAULT);

    // Check that the IO register pins, aka the direction, default to input (1)
    ASSERT_EQ(read_register(&pex, PEX_IODIR_A), 0xEE);
    ASSERT_EQ(read_register(&pex, PEX_IODIR_B), 0xEE);

    // Not sure what the default states of the GPIO pins are
}

// Test that registers can be written to
void write_register_test(void) {
    write_register(&pex, PEX_GPIO_A, 0x00);
    write_register(&pex, PEX_GPIO_B, 0x67); // 0x67 chosen arbitrarily

    ASSERT_EQ(read_register(&pex, PEX_GPIO_A), 0x00);
    ASSERT_EQ(read_register(&pex, PEX_GPIO_B), 0x67);
}

// Test setting specific pins on all registers
void set_pin_test(void) {
    // Set some directions
    pex_set_pin_dir(&pex, 3, PEX_A, OUTPUT);
    pex_set_pin_dir(&pex, 6, PEX_B, OUTPUT);

    ASSERT_EQ(read_register(&pex, PEX_IODIR_A), 0xE5);
    ASSERT_EQ(read_register(&pex, PEX_IODIR_B), 0xCE);

    // Set some states
    pex_set_pin(&pex, 6, PEX_A, HIGH);
    pex_set_pin(&pex, 7, PEX_A, LOW);
    pex_set_pin(&pex, 2, PEX_B, HIGH);
    pex_set_pin(&pex, 1, PEX_B, LOW);

    ASSERT_EQ(pex_get_pin(&pex, 6, PEX_A), HIGH);
    ASSERT_EQ(pex_get_pin(&pex, 7, PEX_A), LOW);
    ASSERT_EQ(pex_get_pin(&pex, 2, PEX_B), HIGH);
    ASSERT_EQ(pex_get_pin(&pex, 1, PEX_B), LOW);

    // Test setting pins that don't exist, values shouldn't change
    pex_set_pin_dir(&pex, 10, PEX_B, OUTPUT);
    ASSERT_EQ(read_register(&pex, PEX_IODIR_B), 0xCE);
}

// Test setting pins and then setting the same pin again
void set_pin_multiple_test(void) {
    // Set the pins
    pex_set_pin(&pex, 3, PEX_A, LOW);
    pex_set_pin(&pex, 4, PEX_A, HIGH);
    pex_set_pin(&pex, 5, PEX_B, LOW);
    pex_set_pin(&pex, 6, PEX_B, HIGH);

    ASSERT_EQ(pex_get_pin(&pex, 3, PEX_A), LOW);
    ASSERT_EQ(pex_get_pin(&pex, 4, PEX_A), HIGH);
    ASSERT_EQ(pex_get_pin(&pex, 5, PEX_B), LOW);
    ASSERT_EQ(pex_get_pin(&pex, 6, PEX_B), HIGH);

    // Set them back
    pex_set_pin(&pex, 3, PEX_A, HIGH);
    pex_set_pin(&pex, 4, PEX_A, LOW);
    pex_set_pin(&pex, 5, PEX_B, HIGH);
    pex_set_pin(&pex, 6, PEX_B, LOW);

    ASSERT_EQ(pex_get_pin(&pex, 3, PEX_A), HIGH);
    ASSERT_EQ(pex_get_pin(&pex, 4, PEX_A), LOW);
    ASSERT_EQ(pex_get_pin(&pex, 5, PEX_B), HIGH);
    ASSERT_EQ(pex_get_pin(&pex, 6, PEX_B), LOW);
}

test_t t1 = { .name = "init_pex", .fn = init_pex_test };
test_t t2 = { .name = "write registers", .fn = write_register_test };
test_t t3 = { .name = "set pins", .fn = set_pin_test };
test_t t4 = { .name = "set pins multiple times", .fn = set_pin_multiple_test };

test_t* suite[4] = {&t1, &t2, &t3, &t4};

int main(void) {
    init_spi();
    run_tests(suite, 4);
    return 0;
}
