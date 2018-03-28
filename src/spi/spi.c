#include <spi/spi.h>

// SPI pins
#define CLK PB7
#define MISO PB0
#define MOSI PB1
#define SS PD3

void init_cs(uint8_t pin, port_t ddr) {
    *ddr |= _BV(pin);
}

void set_cs_low(uint8_t pin, port_t port) {
    *port &= ~(_BV(pin));
}

void set_cs_high(uint8_t pin, port_t port) {
    *port |= _BV(pin);
}

// Before SPI is enables, PPRSPI must be 0
void init_spi(void) {
    // make CLK, MOSI, and SS pins output
    DDRB |= _BV(CLK) | _BV(MOSI);
    DDRD |= _BV(SS);
    // enable SPI, set mode to master, set SCK freq to f_io/64
    SPCR |= _BV(SPE) | _BV(MSTR) | _BV(SPR1);
}

uint8_t send_spi(uint8_t cmd) {
    // TODO: the slave device data mode must match the configured data mode;
    // this often works with the defaults, not not always.
    int TIMEOUT = 65535;
    SPDR = cmd;
    while (!(SPSR & _BV(SPIF)) && TIMEOUT--);
    return SPDR;
}
