#include <test/test.h>
#include <pex/pay.h>
#include <pex/pex.h>

/*
    This test must be run on the PAY PCB.
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
// Also initializes SPI
void init_pex_test(void) {
    init_spi();
    init_pex(&pex);

    ASSERT_EQ(read_register(&pex, PEX_IOCON), PEX_IOCON_DEFAULT);

    // Check that the IO register pins, aka the direction, default to input (1)
    ASSERT_EQ(read_register(&pex, PEX_IODIR_A), 0xFF);
    ASSERT_EQ(read_register(&pex, PEX_IODIR_B), 0xFF);

    // Not sure what the default states of the GPIO pins are
}

// Test that registers can be written to
void write_register_test(void) {
    write_register(&pex, PEX_IODIR_A, 0x0F);
    write_register(&pex, PEX_IODIR_B, 0x67); // 0x67 chosen arbitrarily

    ASSERT_EQ(read_register(&pex, PEX_IODIR_A), 0x0F);
    ASSERT_EQ(read_register(&pex, PEX_IODIR_B), 0x67);
}

// Test setting specific pins on all registers
void set_pin_test(void) {
    // Set some directions
    pex_set_pin_dir(&pex, PEX_A, 3, OUTPUT);
    pex_set_pin_dir(&pex, PEX_B, 6, OUTPUT);

    ASSERT_EQ(read_register(&pex, PEX_IODIR_A), 0x07);
    ASSERT_EQ(read_register(&pex, PEX_IODIR_B), 0x27);

    // Set the direction register to output so we can set values for pins
    write_register(&pex, PEX_IODIR_A, 0x00);
    write_register(&pex, PEX_IODIR_B, 0x00);

    // Set some states
    pex_set_pin(&pex, PEX_A, 6, 1);
    pex_set_pin(&pex, PEX_A, 7, 0);
    pex_set_pin(&pex, PEX_B, 2, 1);
    pex_set_pin(&pex, PEX_B, 1, 0);

    ASSERT_EQ(pex_get_pin(&pex, PEX_A, 6), 1);
    ASSERT_EQ(pex_get_pin(&pex, PEX_A, 7), 0);
    ASSERT_EQ(pex_get_pin(&pex, PEX_B, 2), 1);
    ASSERT_EQ(pex_get_pin(&pex, PEX_B, 1), 0);

    // Test setting pins that don't exist, values shouldn't change
    pex_set_pin_dir(&pex, PEX_B, 10, OUTPUT);
    ASSERT_EQ(read_register(&pex, PEX_IODIR_B), 0x00);
}

// Test setting pins and then setting the same pin again
void set_pin_multiple_test(void) {
    // Set the pins
    pex_set_pin(&pex, PEX_A, 3, 0);
    pex_set_pin(&pex, PEX_A, 4, 1);
    pex_set_pin(&pex, PEX_B, 5, 0);
    pex_set_pin(&pex, PEX_B, 6, 1);

    ASSERT_EQ(pex_get_pin(&pex, PEX_A, 3), 0);
    ASSERT_EQ(pex_get_pin(&pex, PEX_A, 4), 1);
    ASSERT_EQ(pex_get_pin(&pex, PEX_B, 5), 0);
    ASSERT_EQ(pex_get_pin(&pex, PEX_B, 6), 1);

    // Set them back
    pex_set_pin(&pex, PEX_A, 3, 1);
    pex_set_pin(&pex, PEX_A, 4, 0);
    pex_set_pin(&pex, PEX_B, 5, 1);
    pex_set_pin(&pex, PEX_B, 6, 0);

    ASSERT_EQ(pex_get_pin(&pex, PEX_A, 3), 1);
    ASSERT_EQ(pex_get_pin(&pex, PEX_A, 4), 0);
    ASSERT_EQ(pex_get_pin(&pex, PEX_B, 5), 1);
    ASSERT_EQ(pex_get_pin(&pex, PEX_B, 6), 0);
}

test_t t1 = { .name = "init_pex", .fn = init_pex_test };
test_t t2 = { .name = "write registers", .fn = write_register_test };
test_t t3 = { .name = "set pins", .fn = set_pin_test };
test_t t4 = { .name = "set pins multiple times", .fn = set_pin_multiple_test };

test_t* suite[4] = {&t1, &t2, &t3, &t4};

int main(void) {
    run_tests(suite, 4);
    return 0;
}
