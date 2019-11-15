/*
Test BADISR functionality.
*/

#include <utilities/utilities.h>

int main() {
    init_uart();
    print("\n\nStarting test\n");

    // Call some function from utilities.c to make sure the linker includes that
    // file (and therefore the BADISR handler)
    read_eeprom(0x00);

    // Enable UART transmit performed interrupt (p.294)
    // We don't normally use this so there is no ISR defined
    LINENIR |= _BV(LENTXOK);

    // Keep trying to print stuff, see if it outputs "ERROR: BADISR"
    while(1) {
        print("testing\n");
    }
}
