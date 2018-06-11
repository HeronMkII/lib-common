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
#define PRINT_BUF_SIZE 50

//#define UART_TX PD3
//#define UART_RX PD4

// UART
typedef uint8_t(*global_rx_cb_t)(const uint8_t*, uint8_t);

void get_char(uint8_t*);
void register_callback(global_rx_cb_t);
void clear_rx_buffer();

void put_char(uint8_t);
void init_uart();
void send_uart(const uint8_t*, int);

// Printing
int print(char*, ...);
int uprintf(char*, ...);

#endif // UART_H
