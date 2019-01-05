#include <uart/uart.h>

int main(void) {
    init_uart();

    // Uncomment one of these lines to test a different baud rate (default 9600)
    // set_uart_baud_rate(UART_BAUD_1200);
    // set_uart_baud_rate(UART_BAUD_9600);
    // set_uart_baud_rate(UART_BAUD_19200);
    // set_uart_baud_rate(UART_BAUD_115200);

    for(;;) {
        print("The quick brown fox jumps over the lazy dog.\n");
    }
}
