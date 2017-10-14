#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdio.h>

#define F_IO 1000000UL // 1 MHz after CKDIV8
#define BAUD_RATE 9600UL
#define BIT_SAMPLES 4UL

#define UART_TX PD3
#define UART_RX PD4


typedef void(*global_rx_cb_t)(uint8_t*, uint8_t);
void (*global_rx_cb)(uint8_t*, uint8_t);

uint8_t get_char();
void register_callback(global_rx_cb_t);
void clear_rx_buffer();

void put_char(const uint8_t);
void init_uart();
void send_uart(const uint8_t*);
