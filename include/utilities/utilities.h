#ifndef UTILITIES_H
#define UTILITIES_H

#include <avr/eeprom.h>
#include <avr/io.h>

// CPU clock frequency of 8 MHz
// Needs to be defined for using delay functions in the <util/delay.h> library
#ifndef F_CPU
#define F_CPU 8000000UL
#endif
#include <util/delay.h>

// Default values in EEPROM for different sizes
// Byte (1 byte)
#define EEPROM_DEF_BYTE 0xFF
// Word (2 bytes)
#define EEPROM_DEF_WORD 0xFFFF
// Double word (4 bytes)
#define EEPROM_DEF_DWORD 0xFFFFFFFF


// Memory-mapped register types for PORTx and DDRx
// Volatile because the register hardware can change without the program knowing
typedef volatile uint8_t* port_t;
typedef volatile uint8_t* ddr_t;

// Groups the variables needed to manipulate (read/write) a pin
typedef struct {
    port_t port;    // PORTx register - data values
    ddr_t ddr;      // DDRx (data direction register) - input/output directions
    uint8_t pin;    // Pin number (index)
} pin_info_t;

void init_output_pin(uint8_t pin, ddr_t ddr, uint8_t init_val);
void set_pin_low(uint8_t pin, port_t port);
void set_pin_high(uint8_t pin, port_t port);

void init_input_pin(uint8_t pin, ddr_t ddr);
void set_pin_pullup(uint8_t pin, port_t port, uint8_t value);
uint8_t get_pin_val(uint8_t pin, port_t port);

uint32_t read_eeprom(uint16_t addr, uint32_t default_value);
void write_eeprom(uint16_t addr, uint32_t value);

#endif // UTILITIES_H
