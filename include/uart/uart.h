#ifndef UART_H
#define UART_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>


/*
Available baud rates to set for UART
Currently matches the available baud rates for the transceiver
Can add more options later if the clock division ratios work out
*/
typedef enum {
    UART_BAUD_1200,
    UART_BAUD_9600,
    UART_BAUD_19200,
    UART_BAUD_115200
} uart_baud_rate_t;

/*
clk_io frequency (p. 17, 48, 54, 143, 280, 282, 298)
With the current fuse settings (FF D7 FF), the CKDIV8 bit is not programmed.
It seems like there is no clock divison for clk_io (therefore 8 MHz).
This is probably better than 1 MHz so we can reach higher UART baud rates.
*/
#define UART_F_IO 8000000UL

// Default baud rate (number of characters per second)
// p. 282, 298
#define UART_DEF_BAUD_RATE UART_BAUD_9600

// UART TXD is pin PD3
// UART RXD is pin PD4

// UART RX callback function signature
typedef uint8_t(*uart_rx_cb_t)(const uint8_t*, uint8_t);


// UART RX/TX (from uart.c)
void init_uart(void);
void set_uart_baud_rate(uart_baud_rate_t baud_rate);
void put_uart_char(uint8_t c);
void get_uart_char(uint8_t* c);
void send_uart(const uint8_t* msg, uint8_t len);
void set_uart_rx_cb(uart_rx_cb_t cb);
void clear_uart_rx_buf(void);

// Printing (from log.c)
int16_t print(char* str, ...);
void print_bytes(uint8_t* data, uint8_t len);

#endif // UART_H
