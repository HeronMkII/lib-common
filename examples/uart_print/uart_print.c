#include <uart/uart.h>

int main(void) {
    init_uart();

    // Uncomment to test a different baud rate
    // set_uart_baud_rate(UART_BAUD_19200);

    for(;;) {
        print("The quick brown fox jumps over the lazy dog.\n");
    }
}
