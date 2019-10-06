/*
Utilities library

Common utilities for using the microcontroller such as delays and pin manipulations.
*/

#include <utilities/utilities.h>

/*
Initializes a pin on the microcontroller as an output pin with a default value.
pin - Pin number
ddr - DDR register
init_val - Initial value to set the pin to (either 0 or 1)
*/
void init_output_pin(uint8_t pin, ddr_t ddr, uint8_t init_val) {
    // Set DDR (data direction register) bit to 1 (output)
    *ddr |= _BV(pin);

    // We should try not to break code by having an extra `port` function
    // parameter, so match it from `ddr`
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

    // Set the initial output value using the port register
    if (init_val == 0) {
        set_pin_low(pin, port);
    } else if (init_val == 1) {
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


/*
Initializes a pin on the microcontroller as an input pin.
pin - Pin number
ddr - DDR register
*/
void init_input_pin(uint8_t pin, ddr_t ddr) {
    // Set DDR (data direction register) bit to 0 (input)
    *ddr &= ~_BV(pin);
}

/*
Enables or disables the pullup resistor on an input pin.
pin - Pin number
port - PORT register
value - 0 (tri-state) or 1 (pullup)
*/
void set_pin_pullup(uint8_t pin, port_t port, uint8_t value) {
    if (value) {
        *port |= _BV(pin);
    } else {
        *port &= ~_BV(pin);
    }
}

/*
Gets an input pin's value.
pin - Pin number
port - PORT register
Returns - 0 (low) or 1 (high)
*/
uint8_t get_pin_val(uint8_t pin, port_t port) {
    // We should try not to break code by switching `port` to `pin`, so match it
    if        (port == &PORTB) {
        return (PINB & _BV(pin)) ? 1 : 0;
    } else if (port == &PORTC) {
        return (PINC & _BV(pin)) ? 1 : 0;
    } else if (port == &PORTD) {
        return (PIND & _BV(pin)) ? 1 : 0;
    } else if (port == &PORTE) {
        return (PINE & _BV(pin)) ? 1 : 0;
    } else {
        return 0;
    }
}

/*
Reads a dword (i.e. double word, 4 bytes) from the specified address in EEPROM.
If it reads the default value after erasing (0xFFFFFFFF), returns the specified
default value instead.
addr - 16 bits because uint32_t* pointers are represented as a 16-bit address
*/
uint32_t read_eeprom(uint16_t addr, uint32_t default_value) {
    uint32_t value = eeprom_read_dword((uint32_t*) addr);
    if (value == EEPROM_DEF_DWORD) {
        return default_value;
    }
    return value;
}

void write_eeprom(uint16_t addr, uint32_t value) {
    // Should use eeprom_update_dword instead of eeprom_write_dword, since it
    // reads the current value and only writes if the new value is different
    // This reduces wear on the devices from the number of writes
    // See https://www.avrfreaks.net/sites/default/files/forum_attachments/EEPROM.pdf, Section 2.2
    eeprom_update_dword((uint32_t*) addr, value);
}
