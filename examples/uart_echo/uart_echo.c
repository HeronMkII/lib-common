#include <uart/uart.h>

/* To run this example, connect the TX pin of the programmer to the SCK pin on
 * the board. Because the SCK pin is also used by SPI when programming the
 * board, the TX pin on the programmer cannot be connected to SCK when
 * uploading new programs.
 */

void echo(uint8_t *buf, uint8_t len) {
    for (uint8_t i = 0; i < len; i++) {
        put_char(buf[i]);
    }

    clear_rx_buffer();
}

int main(void) {
    init_uart();
    register_callback(echo);

    while (1) {};
}
