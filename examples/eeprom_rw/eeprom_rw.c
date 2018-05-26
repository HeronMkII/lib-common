#include <avr/eeprom.h>

/*
 * Run the following command in the command line:
 *
 *  $ avrdude -p m32m1 -c stk500 -P /dev/tty.usbmodem00208212 -U eeprom:r:eeprom_contents.bin:r
 *
 * This creates a file called eeprom_contents.bin, which contains a raw binary
 * copy of the micro's EEPROM memory.
 *
 * Next, to view the contents of this binary file, run
 *
 *  $ hexdump eeprom_contents.bin
 *
 * You should see 0xbb at position 47.
 */

int main(void) {
    uint8_t* addr = (uint8_t *)47;
    uint8_t data = 0xbb;

    eeprom_update_byte(addr, data);
    return 0;
}

