#include <uart/uart.h>

/* To run this example, connect the TX pin of the programmer to the SCK pin on
 * the board. Because the SCK pin is also used by SPI when programming the
 * board, the TX pin on the programmer cannot be connected to SCK when
 * uploading new programs.
 */

// FIXME: When running this in line mode using CoolTerm, the 11th character
// of any string is consistently dropped. When sending the characters with
// a 3ms delay, this effect disappears.

// UPDATE (2018-12-19): By changing the LDIV formula in init_uart() from
// (... - 1) to (... - 2), this effect occurs for every 17th or 18th character instead
// of every 11th. This a temporary improvement, but still needs to be
// investigated. Perhaps the UART RX and TX are conflicting and received data is
// being lost in the buffer?

uint8_t echo(const uint8_t* buf, uint8_t len) {
    for (uint8_t i = 0; i < len; i++) {
        put_char(buf[i]);
    }

    return len;
}

int main(void) {
    init_uart();
    set_uart_rx_cb(echo);

    while (1) {};
}
