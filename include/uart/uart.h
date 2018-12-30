#ifndef UART_H
#define UART_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

/*
clk_io frequency (p. 17, 48, 54, 143, 280, 282, 298)
With the current fuse settings (FF D7 FF), the CKDIV8 bit is not programmed.
It seems like there is no clock divison for clk_io (therefore 8 MHz).
This is probably better than 1 MHz so we can reach higher UART baud rates.
*/
#define UART_F_IO 8000000UL

// Default baud rate (number of characters per second)
// This needs to match the baud rate for the transceiver/CoolTerm
// p. 282, 298
#define UART_DEF_BAUD_RATE 9600UL

// LBT[5:0] value in LINBTR (LIN bit timing) register (p. 283, 289, 297)
// LBT[5:0] can't be changed in the register unless the LDISR
// (disable bit timing resynchronization) bit is set to 1 in the LINBTR register
// (we are not changing it)
#define UART_BIT_SAMPLES 32UL

// UART TXD is pin PD3
// UART RXD is pin PD4


// UART RX callback function signature
typedef uint8_t(*uart_rx_cb_t)(const uint8_t*, uint8_t);

// UART RX/TX (from uart.c)
void put_uart_char(uint8_t c);
void get_uart_char(uint8_t* c);
void init_uart(void);
void send_uart(const uint8_t* msg, uint8_t len);
void set_uart_rx_cb(uart_rx_cb_t cb);
void clear_uart_rx_buf(void);

// Printing (from log.c)
int16_t print(char*, ...);
void print_bytes(uint8_t*, uint8_t);

#endif // UART_H
