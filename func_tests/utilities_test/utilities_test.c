#include <utilities/utilities.h>

int main(void) {
    init_uart();
    print("Init\n");

    // Test printing
    uint8_t data[] = { 0x71, 0xFF, 0x00 };
    print_hex_bytes(data, sizeof(data) / sizeof(data[0]));

    // Test that we imported <util/delay.h> and used the right F_CPU
    print("Delaying 5s\n");
    _delay_ms(5000);

    print("Done\n");
}
