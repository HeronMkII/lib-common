#include <uart/uart.h>

int main(void) {
    init_uart();

    for(;;) {
        print("The quick brown fox jumps over the lazy dog.\n");
    }
}
