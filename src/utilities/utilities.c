/*
A collection of miscellanous utilities (useful constants, includes, and functions)
between subsystems.
*/

#include <utilities/utilities.h>

// Prints an array of bytes in hex format on one line
// (e.g. 0x4A 0x71 0xFF)
void print_hex_bytes(uint8_t *data, uint8_t len) {
    for (uint8_t i = 0; i < len; i++) {
        print("0x%02x ", data[i]);
    }
    print("\n");
}
