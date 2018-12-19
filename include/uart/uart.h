#ifndef UART_H
#define UART_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

// With the current fuse settings (FF D7 FF), the CKDIV8 bit is not programmed.
// It seems like there is no clock divison for clk_io (therefore 8 MHz).
// TODO - should it be 1 MHz after CKDIV8?
// clk_io frequency (p. 17, 48, 54, 143, 280, 282, 298)
#define F_IO 8000000UL

// Baud rate (number of samples per second)
// This needs to match the baud rate for the transceiver/CoolTerm
// p. 282, 298
#define BAUD_RATE 9600UL

// LBT[5:0] value in LINBTR register (p. 283, 289, 297)
#define BIT_SAMPLES 32UL

// UART TXD is pin PD3
// UART RXD is pin PD4


// UART RX callback function signature
typedef uint8_t(*uart_rx_cb_t)(const uint8_t*, uint8_t);

// UART RX/TX (from uart.c)
void put_char(uint8_t);
void get_char(uint8_t*);
void init_uart(void);
void send_uart(const uint8_t*, uint8_t);
void set_uart_rx_cb(uart_rx_cb_t);
void clear_rx_buffer(void);

// Printing (from log.c)
int16_t print(char*, ...);
void print_bytes(uint8_t*, uint8_t);

#endif // UART_H
