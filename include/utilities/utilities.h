#ifndef UTILITIES_H
#define UTILITIES_H

// Need to indicate how fast we know the CPU clock is running (8MHz)
// This is used for <util/delay.h>
// Define this here so we don't need to redefine it in many files
#ifndef F_CPU
#define F_CPU 8000000UL
#endif
#include <util/delay.h>

#include <uart/uart.h>

void print_hex_bytes(uint8_t *data, uint8_t len);

#endif
