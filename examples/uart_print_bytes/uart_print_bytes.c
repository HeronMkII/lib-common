#include <uart/uart.h>

int main(void) {
    init_uart();

    print("\n\nStart\n");

    uint8_t bytes_1[6] = {0x41, 0xA3, 0xFF, 0x00, 0x07, 0xB0};
    print_bytes(bytes_1, 6);

    // Test length >= 256 (for uint16_t instead of uint8_t)
    // Should be 0, 3, 6, ..., 255, 2, 5, 8, ..., 254, 1, ..., 253, 0, ..., 129
    uint8_t bytes_2[300] = {0x00};
    for (uint16_t i = 0; i < 300; i++) {
        bytes_2[i] = i * 3;
    }
    print_bytes(bytes_2, 300);

    print("Done\n");
}
