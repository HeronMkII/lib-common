#ifndef UTILITIES_H
#define UTILITIES_H

#include <avr/io.h>

// CPU clock frequency of 8 MHz
// Needs to be defined for using delay functions in the <util/delay.h> library
#ifndef F_CPU
#define F_CPU 8000000UL
#endif
#include <util/delay.h>


// Memory-mapped register types for PORTx and DDRx
// Volatile because the register hardware can change without the program knowing
typedef volatile uint8_t* port_t;
typedef volatile uint8_t* ddr_t;

// Groups the variables needed to manipulate (read/write) a pin
typedef struct {
    port_t port;
    ddr_t ddr;
    uint8_t pin;
} pin_info_t;

// Possible values for an output pin
typedef enum {
    PIN_LOW = 0,
    PIN_HIGH = 1
} pin_val_t;

void init_output_pin(uint8_t pin, ddr_t ddr, pin_val_t default_val);
void set_pin_low(uint8_t pin, port_t port);
void set_pin_high(uint8_t pin, port_t port);

#endif // UTILITIES_H
