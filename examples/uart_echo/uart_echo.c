#include <uart/uart.h>

/* To run this example, connect the TX pin of the programmer to the SCK pin on
 * the board. Because the SCK pin is also used by SPI when programming the
 * board, the TX pin on the programmer cannot be connected to SCK when
 * uploading new programs.
 */

// FIXME: When running this in line mode using CoolTerm, the 11th character
// of any string is consistently dropped. When sending the characters with
// a 3ms delay, this effect disappears.

uint8_t echo(const uint8_t* buf, uint8_t len) {
    for (uint8_t i = 0; i < len; i++) {
        put_char(buf[i]);
    }

    return len;
}

int main(void) {
    init_uart();
    register_callback(echo);

    while (1) {};
}
