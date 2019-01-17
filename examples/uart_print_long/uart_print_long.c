/*
Tests printing a long string, longer than PRINT_BUF_SIZE (50 characters).
Check that the behaviour of vsnprintf will cut off the string after 49
characters (before the NULL character) instead of trying to print the whole thing.
*/

#include <uart/uart.h>

int main(void) {
    init_uart();

    // Uncomment one of these lines to test a different baud rate (default 9600)
    // set_uart_baud_rate(UART_BAUD_1200);
    // set_uart_baud_rate(UART_BAUD_9600);
    // set_uart_baud_rate(UART_BAUD_19200);
    // set_uart_baud_rate(UART_BAUD_115200);

    for(;;) {
        print("This is a long string, a very long string, a very very long string");
        print("\n");
    }
}
