/*
UART library logging
Functions for using variable arguments and format specifiers to print messages.
*/

#include <uart/uart.h>

// Character buffer for formatted print messages
#define PRINT_BUF_SIZE 50
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
inline int16_t print(char* str, ...) {
    va_list args;
    va_start(args, str);

    int16_t ret = vsprintf((char*) print_buf, str, args);
    va_end(args);

    send_uart(print_buf, strlen((char*) print_buf));
    return ret;
}

/*
Prints an array of bytes in hex format on the same line.
data - pointer to beginning of array
len - number of bytes in array
*/
void print_bytes(uint8_t* data, uint8_t len) {
    for (uint8_t i = 0; i < len; i++) {
        print("0x%.2x ", data[i]);
    }

    print("\n");
}
