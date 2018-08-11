/*
MCP23S17 port expander (PEX)
Datasheet: http://ww1.microchip.com/downloads/en/DeviceDoc/20001952C.pdf

A port expander is a device with many GPIO (general purpose input/output) pins.
Each GPIO pin can function as either an input or an output, depending on what
you want to use it for. Using a port expander gives us more GPIO pins to work
with since we have a limited number on the 32M1 itself.

The 32M1 communicates with the PEX over SPI, telling it to use pins as inputs or
outputs, read values from pins, or write values to pins.

The PEX has two "banks", labelled A and B. Each has 8 pins,
labelled GPA0-GPA7 and GPB0-GPB7.

Each PEX device has a 3-bit address, determined by how the pins A2, A1, A0
are connected in hardware. Each SPI command contains the address of the PEX it
is meant for, allowing up to 8 PEX devices to be connected using the same CS
(chip select) line. A PEX will only respond to the command if its address
matches the one sent in the comand.

AUTHORS: Dylan Vogel, Shimi Smith, Bruno Almeida, Siddharth Mahendraker
*/

#include <pex/pex.h>

#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include <util/delay.h>


// TODO: clarify this section
// Register addresses
#define PEX_IOCON       0x0A // Assumes bank = 0, such as after reset
#define PEX_IODIR_BASE  0x00 // where direction is stored. 0 is output, 1 is input.
#define PEX_GPIO_BASE   0x12 // where GPIO states are stored.

// Register addresses for banks A and B
#define PEX_IODIR_A (PEX_IODIR_BASE)
#define PEX_IODIR_B (PEX_IODIR_BASE + 0x01)
#define PEX_GPIO_A  (PEX_GPIO_BASE)
#define PEX_GPIO_B  (PEX_GPIO_BASE + 0x01)

// Default configuration
#define PEX_IOCON_DEFAULT       0b00001000
// Bit 3 sets hardware addressing

// Control bytes for writing and reading registers
#define PEX_WRITE_CONTROL_BYTE  0b01000000
#define PEX_READ_CONTROL_BYTE   0b01000001
// Bits [3:1] are A[2:0] hardware addresses.

// Initializes port expander reset and chip select pins on the 32M1
void init_pex(pex_t* pex) {
    init_cs(pex->rst->pin, pex->rst->ddr);
    set_cs_high(pex->rst->pin, pex->rst->port);

    init_cs(pex->cs->pin, pex->cs->ddr);
    set_cs_high(pex->cs->pin, pex->cs->port);

    // Default configuration
    write_register(pex, PEX_IOCON, PEX_IOCON_DEFAULT);
}

// Resets the port expander
void reset_pex(pex_t* pex) {
    set_cs_low(pex->rst->pin, pex->rst->port);
    _delay_ms(1); // minimum 1 microsecond
    set_cs_high(pex->rst->pin, pex->rst->port);
    _delay_ms(1);
}

 // Writes data 'data' to register 'addr'
void write_register(pex_t* pex, uint8_t addr, uint8_t data) {
    set_cs_low(pex->cs->pin, pex->cs->port);
    send_spi(PEX_WRITE_CONTROL_BYTE | (pex->addr << 1));
    send_spi(addr);
    send_spi(data);
    set_cs_high(pex->cs->pin, pex->cs->port);
}

// Reads data from register 'addr'
uint8_t read_register(pex_t* pex, uint8_t addr) {
    set_cs_low(pex->cs->pin, pex->cs->port);
    send_spi(PEX_READ_CONTROL_BYTE | (pex->addr << 1));
    send_spi(addr);
    uint8_t ret = send_spi(0x00);
    set_cs_high(pex->cs->pin, pex->cs->port);

    return ret;
}

// Sets the direction of pin 'pin' on bank 's' to state 'dir'
// bank - A (GPIOA) or B (GPIOB)
// direction - OUTPUT or INPUT
void pex_set_pin_dir(pex_t* pex, uint8_t pin, pex_bank_t s, pex_dir_t dir) {
    uint8_t base = PEX_IODIR_BASE + s;
    uint8_t register_state = read_register(pex, base);
    switch (dir) {
        case OUTPUT:
            write_register(pex, base, (register_state & ~_BV(pin)));
            break;
        case INPUT:
            write_register(pex, base, (register_state | _BV(pin)));
            break;
    }
}

// Sets the value of pin 'pin' on bank 's' to value 'v'
// bank - A (GPIOA) or B (GPIOB)
// value - HIGH or LOW
void pex_set_pin(pex_t* pex, uint8_t pin, pex_bank_t s, pex_val_t v) {
    uint8_t base = PEX_GPIO_BASE + s;
    uint8_t register_state = read_register(pex, base);
    switch (v) {
        case HIGH:
            write_register(pex, base, register_state | _BV(pin));
            break;
        case LOW:
            write_register(pex, base, register_state & ~_BV(pin));
            break;
    }
}

// Reads the state of `pin` on bank `s` (either 0 or 1)
// bank - A (GPIOA) or B (GPIOB)
uint8_t pex_get_pin(pex_t* pex, uint8_t pin, pex_bank_t s) {
    uint8_t base = PEX_GPIO_BASE + s;
    uint8_t register_state = read_register(pex, base);
    return (register_state >> pin) & 0b1;
}
