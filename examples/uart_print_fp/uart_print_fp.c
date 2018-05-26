#include <uart/uart.h>

int main(void) {
    init_uart();
    float pi = 3.14159;

    for(;;) {
        print("pi = %.5f\n", pi);
    }
}
