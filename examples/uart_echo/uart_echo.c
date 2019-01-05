#include <uart/uart.h>

/* To run this example, connect the TX pin of the programmer to the SCK pin on
 * the board. Because the SCK pin is also used by SPI when programming the
 * board, the TX pin on the programmer cannot be connected to SCK when
 * uploading new programs.
 */

/*
When running this in line mode using CoolTerm (sending an entire message at once
using the Connection > Send String... dialog), every 11th character of any
string is consistently dropped (i.e. 11 characters work, 1 dropped, next 11
characters work, 1 dropped, and so on). When sending the characters with a 3ms
delay (going to Options > Transmit and checking the "Use transmit character
delay" box at the top), this effect disappears.

Update (2018-12-30):
The current best explanation of this is that whenever the callback function in
this example received a single character, it sends it back. There are common
resources shared between UART RX and TX (e.g. the data register and the busy
bit), so sometimes they conflict. Every 11 characters, the alternating between
receiving a character and sending a character causes a timing problem so one
character fails. This can be fixed by defining the callback function in a way
that it waits for some sort of line termination (e.g. '\r' or '\n'), then sends
all the characters back at once.
Alternatively, this could have been because it took a long time to shift all the
characters in the UART buffer left every time a character was removed.
*/

uint8_t echo(const uint8_t* buf, uint8_t len) {
    for (uint8_t i = 0; i < len; i++) {
        put_uart_char(buf[i]);
    }

    return len;
}

int main(void) {
    init_uart();
    set_uart_rx_cb(echo);

    // Uncomment one of these lines to test a different baud rate (default 9600)
    // set_uart_baud_rate(UART_BAUD_1200);
    // set_uart_baud_rate(UART_BAUD_9600);
    // set_uart_baud_rate(UART_BAUD_19200);
    // set_uart_baud_rate(UART_BAUD_115200);

    while (1) {};
}
