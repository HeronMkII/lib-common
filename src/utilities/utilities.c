/*
Utilities library

Common utilities for using the microcontroller such as delays and pin manipulations.
*/

#include <utilities/utilities.h>

/*
Initializes a pin on the microcontroller as an output pin with a default value.
pin - Pin number
ddr - DDR register
default_val - Default (initial) value to set the pin to (either PIN_LOW or PIN_HIGH)
*/
void init_output_pin(uint8_t pin, ddr_t ddr, pin_val_t default_val) {
    // Set DDR (data direction register) bit to 1 (output)
    *ddr |= _BV(pin);

    // TODO - is there a better way to do this?
    // We should try to not have an extra `port` function parameter, so match it
    // from `ddr`
    port_t port;
    if (ddr == &DDRB) {
        port = &PORTB;
    } else if (ddr == &DDRC) {
        port = &PORTC;
    } else if (ddr == &DDRD) {
        port = &PORTD;
    } else if (ddr == &DDRE) {
        port = &PORTE;
    } else {
        return;
    }

    // Set the default output value using the port register
    if (default_val == PIN_LOW) {
        set_pin_low(pin, port);
    } else if (default_val == PIN_HIGH) {
        set_pin_high(pin, port);
    }
}

/*
Sets an output pin's value to be low.
pin - Pin number
port - PORT register
*/
void set_pin_low(uint8_t pin, port_t port) {
    *port &= ~_BV(pin);
}

/*
Sets an output pin's value to be high.
pin - Pin number
port - PORT register
*/
void set_pin_high(uint8_t pin, port_t port) {
    *port |= _BV(pin);
}
