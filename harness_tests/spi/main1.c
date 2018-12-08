#include <test/test.h>
#include <spi/spi.h>

#define CLK PB7
#define MISO PB0
#define MOSI PB1
#define SS PD3

#define PBT 4 // test CS pin

void init_cs_test(void) {
    // check that the chip select (CS) pin is correctly initialized
    // as an output pin
    init_cs(PBT, &DDRB);
    uint8_t pin = DDRB & _BV(PBT);
    ASSERT_EQ(pin, _BV(PBT));
}

void set_cs_low_test(void) {
    // check that pulling CS low works
    set_cs_low(PBT, &PORTB);
    uint8_t pin = PORTB & _BV(PBT);
    ASSERT_EQ(pin, 0);
}

void set_cs_high_test(void) {
    // check that pulling CS high works
    set_cs_high(PBT, &PORTB);
    uint8_t pin = PORTB & _BV(PBT);
    ASSERT_EQ(pin, _BV(PBT));
}

void init_spi_test(void) {
    // check that the MOSI and CLK pins are configured
    // as output pins
    uint8_t pins = DDRB & (_BV(MOSI) | _BV(CLK));
    ASSERT_EQ(pins, _BV(MOSI) | _BV(CLK));

    // check that SS is configured as an output pin
    uint8_t ss = DDRD & _BV(SS);
    ASSERT_EQ(ss, _BV(SS));
    // NOTE: SS is the same this as CS, but it's never actually used

    // check that SPI is enabled (SPE), we are in master mode (MSTR)
    // and set that the appropriate clock freq. has been set (SPR1)
    uint8_t flags = SPCR & (_BV(SPE) | _BV(MSTR) | _BV(SPR1));
    ASSERT_EQ(flags, _BV(SPE) | _BV(MSTR) | _BV(SPR1));
}

test_t t1 = { .name = "init_spi", .fn = init_spi_test };
test_t t2 = { .name = "init_cs", .fn = init_cs_test };
test_t t3 = { .name = "set_cs_low", .fn = set_cs_low_test };
test_t t4 = { .name = "set_cs_high", .fn = set_cs_high_test };

test_t* suite[4] = { &t1, &t2, &t3, &t4 };

int main() {
    init_spi();
    run_tests(suite, 4);
    return 0;
}
