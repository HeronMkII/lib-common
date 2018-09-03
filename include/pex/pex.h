#ifndef PEX_H
#define PEX_H

#include <spi/spi.h>
#include <stdint.h>
#include <avr/io.h>

// Addresses as set by A2-0 pins in hardware
#define PEX_ADDR_PAY        1
#define PEX_ADDR_PAY_OPT    2
#define PEX_ADDR_EPS        0

// Direction of GPIO pins
typedef enum {
    OUTPUT = 0,
    INPUT = 1
} pex_dir_t;

// Bank A or B of GPIO pins
typedef enum {
    PEX_A = 0,
    PEX_B = 1
} pex_bank_t;

// GPIO pin values
typedef enum {
    HIGH = 1,
    LOW = 0
} pex_val_t;

// PEX device
typedef struct {
    uint8_t addr;

    pin_info_t* cs; // chip select (CS) pin info
    pin_info_t* rst; // reset pin info
} pex_t;

void init_pex(pex_t*);
void reset_pex(pex_t*);

uint8_t read_register(pex_t*, uint8_t addr);
void write_register(pex_t*, uint8_t addr, uint8_t data);

void pex_set_pin_dir(pex_t* pex, uint8_t pin, pex_bank_t s, pex_dir_t dir);
void pex_set_pin(pex_t* pex, uint8_t pin, pex_bank_t s, pex_val_t v);
uint8_t pex_get_pin(pex_t* pex, uint8_t pin, pex_bank_t s);

#endif // PEX_H
