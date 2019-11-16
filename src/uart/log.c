/*
UART library logging
Functions for using variable arguments and format specifiers to print messages.
*/

#include <uart/uart.h>

// Character buffer for formatted print messages
uint8_t print_buf[PRINT_BUF_SIZE];

/*
Prints a message by sending UART.
Uses same format specifiers as the standard C printf() function
(http://www.cplusplus.com/reference/cstdio/printf/?kw=printf)

Note: UART must be initialized (with init_uart()) before calling print
Note: Floating point output (with %f) is not available by default, must add
      -lprintf_flt flag to linking command

str - Format string for the message
variable arguments - To be substituted for format specifiers
*/
inline int16_t print(char* fmt, ...) {
    va_list args;
    va_start(args, fmt);

    /*
    Note that we use vsnprintf instead of vsprintf to specify the maximum
    number of characters to be written to print_buf. This is to prevent errors
    if you try to print a string longer than PRINT_BUF_SIZE, exceeding
    print_buf and overwriting uart_rx_buf and/or uart_rx_buf_count.
    See https://www.microchip.com/webdoc/AVRLibcReferenceManual/group__avr__stdio_1gac92e8c42a044c8f50aad5c2c69e638e0.html
    */
    int16_t ret = vsnprintf((char*) print_buf, PRINT_BUF_SIZE, fmt, args);
    va_end(args);

    send_uart(print_buf, strlen((char*) print_buf));
    return ret;
}

/*
Prints an array of bytes in hex format on the same line.
data - pointer to beginning of array
len - number of bytes in array
*/
void print_bytes(uint8_t* data, uint16_t len) {
    if (len == 0) {
        return;
    }
    print("%.2x", data[0]);
    for (uint16_t i = 1; i < len; i++) {
        print(":%.2x", data[i]);
    }
    print("\n");
}

uint8_t* get_print_buf(void) {
    return print_buf;
}
