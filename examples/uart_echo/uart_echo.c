#include <uart/uart.h>

void echo(uint8_t c) {
    put_char(c);
}

int main(void) {
    init_uart();
    register_callback(echo);

    while (1) {};
}
