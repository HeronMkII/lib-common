#include <stdint.h>

int main(void) {
    volatile uint8_t j = 0;
    for (uint8_t i = 0; i < 100; i++) {
        j += i;
    }
    return 0;
}
