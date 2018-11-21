#ifndef UART_H
#define UART_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#define F_IO 1000000UL // 1 MHz after CKDIV8
#define BAUD_RATE 9600UL
#define BIT_SAMPLES 4UL

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
void register_callback(uart_rx_cb_t);
void clear_rx_buffer(void);

// Printing (from log.c)
int16_t print(char*, ...);
void print_bytes(uint8_t*, uint8_t);

#endif // UART_H
