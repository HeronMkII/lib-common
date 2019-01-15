/*
This program checks the sizeof() as defined in AVR for different data types.
sizeof(type) evaluates to a number, the number of bytes used to represent type.
*/

#include <uart/uart.h>
#include <stdint.h>

int main(void){
    init_uart();
    print("\n\n");

    print("sizeof(int) = %u\n", sizeof(int));
    print("sizeof(unsigned int) = %u\n", sizeof(unsigned int));
    print("sizeof(uint8_t) = %u\n", sizeof(uint8_t));
    print("sizeof(uint32_t) = %u\n", sizeof(uint32_t));
    print("sizeof(uint64_t) = %u\n", sizeof(uint64_t));

    print("sizeof(7) = %u\n", sizeof(7));
    print("sizeof(7L) = %u\n", sizeof(7L));
    print("sizeof(7UL) = %u\n", sizeof(7UL));
    print("sizeof(7LL) = %u\n", sizeof(7LL));
    print("sizeof(7ULL) = %u\n", sizeof(7ULL));

    print("\n");
}
