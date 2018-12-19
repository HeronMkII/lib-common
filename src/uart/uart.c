/*
UART (TX and RX) library.

32M1/64M1 datasheet: https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-8209-8-bit%20AVR%20ATmega16M1-32M1-64M1_Datasheet.pdf

TODO - maybe implement functions to get the buffer count and extract
       characters without an interrupt callback?
*/

#include <uart/uart.h>
#include <string.h>

// Maximum number of characters the UART RX buffer can store
#define MAX_RX_BUF_SIZE 50

// Buffer of received characters
// TODO - should this be volatile?
uint8_t uart_rx_buf[MAX_RX_BUF_SIZE];
// Number of valid characters in buffer (starting at index 0)
volatile uint8_t uart_rx_buf_count;

// default rx callback (no operation)
uint8_t _nop(const uint8_t* c, uint8_t len) {
    return 0;
}
// Global RX callback function
uart_rx_cb_t global_rx_cb = _nop;


// Sends one character over UART (TX)
void put_char(uint8_t c) {
    uint16_t timeout = UINT16_MAX;
    while ((LINSIR & _BV(LBUSY)) && timeout--);
    LINDAT = c;
}

// Gets (receives) one character from UART (RX) and sets the value of `c`
// TODO - Maybe change to uint8_t get_char(uint8_t*) and write value directly?
// Frees up ret val for error handling
void get_char(uint8_t* c) {
    uint16_t timeout = UINT16_MAX;
    while ((LINSIR & _BV(LBUSY)) && timeout--);
    *c = LINDAT;
}

// Initializes the UART library
void init_uart(void) {
    LINCR = _BV(LSWRES); // reset UART

    // TODO - 2?
    // Scaling of clk_io frequency
    // Value to assign to 16-bit LINBRR register (p. 298)
    // Calculated from formula on p.282
    int16_t LDIV = (F_IO / (BAUD_RATE * BIT_SAMPLES)) - 2;
    // this number is not necessarily integer; if the number is too far from
    // being an integer, too much error will accumulate and UART will output
    // garbage

    // Set LDIV (clock scaling) value to LINBRR
    // TODO - should these two lines be atomic?
    LINBRRH = (uint8_t) (LDIV >> 8);
    LINBRRL = (uint8_t) LDIV;

    // Set number of samples per bit (p. 297)
    LINBTR = BIT_SAMPLES;

    LINCR = _BV(LENA) | _BV(LCMD2) | _BV(LCMD1) | _BV(LCMD0);
    // enable UART, full duplex

    LINENIR = _BV(LENRXOK);
    global_rx_cb = _nop;
    sei();
    // enable UART interrupts

    clear_rx_buffer();
    // reset RX buffer and counter
}

/*
Sends a sequence of characters over UART.
msg - pointer to start of array
len - number of characters
*/
void send_uart(const uint8_t* msg, uint8_t len) {
    for (uint8_t i = 0; i < len; i++) {
        put_char(msg[i]);
    }
}

/*
Sets the callback function that will be called when UART receives data.

The function must have the following signature:
uint8_t func(const uint8_t* data, uint8_t len);

The UART library will pass a pointer to the array (data) and the number of characters (len).
The function can process the characters however it wants and must return the
number of characters it has "processed", which will then be removed from the uart_rx_buf.
*/
void set_uart_rx_cb(uart_rx_cb_t cb) {
    global_rx_cb = cb;
}

// Clears the RX buffer (sets all values in the arraay to 0 and sets counter to 0)
void clear_rx_buffer(void) {
    uart_rx_buf_count = 0;
    for (uint8_t i = 0; i < MAX_RX_BUF_SIZE; i++) {
        uart_rx_buf[i] = 0;
    }
}

// Interrupt handler that will be called when we receive a character over UART
ISR(LIN_TC_vect) {
    if (LINSIR & _BV(LRXOK)) {
        static uint8_t c;
        get_char(&c);

        uart_rx_buf[uart_rx_buf_count] = c;
        uart_rx_buf_count += 1;

        uint8_t read_bytes = global_rx_cb(uart_rx_buf, uart_rx_buf_count);

        // shift everything leftward by the number of bytes read, if any
        if (read_bytes > 0) {
            memmove(uart_rx_buf, uart_rx_buf + read_bytes, MAX_RX_BUF_SIZE - read_bytes);
            uart_rx_buf_count -= read_bytes;
        }

        // If the buffer is full, clear it
        if (uart_rx_buf_count >= MAX_RX_BUF_SIZE) {
            clear_rx_buffer();
        }

        LINSIR &= ~(_BV(LRXOK)); // clear bit
    }
}
