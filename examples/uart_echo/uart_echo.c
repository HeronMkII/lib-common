#include <uart/uart.h>

void echo(uint8_t *buf, uint8_t len) {
    if (len == 10) {
        for (uint8_t i = 0; i < len; i++)
            put_char(buf[i]);
        clear_rx_buffer();
    }
}

int main(void) {
    init_uart();
    register_callback(echo);

    while (1) {};
}
