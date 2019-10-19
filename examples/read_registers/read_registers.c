/*
This program tries to use the AVR memory mapped I/O to read arbitrary registers.

For example on macOS, can see the header file
usr/local/Cellar/avr-gcc/8.2.0/avr/include/avr/io64m1.h
for memory address definitions for registers and MMIO.
*/

#include <uart/uart.h>
#include <avr/io.h>

void read(uint16_t addr) {
    volatile uint8_t* ptr = (volatile uint8_t*) addr;
    uint8_t value = *ptr;
    print("0x%x: 0x%.2x\n", addr, value);
}

int main(void){
    init_uart();
    print("\n\n");

    for (uint16_t addr = 0; addr < 0x100; addr++) {
        read(addr);
    }

    print("Done\n");
}
