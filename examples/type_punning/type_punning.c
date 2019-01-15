/*
This program experiments with "type punning": https://en.wikipedia.org/wiki/Type_punning
*/

#include <uart/uart.h>
#include <stdint.h>

int main(void){
    init_uart();
    print("\n\n");

    uint8_t unsig = 129;
    print("%u %x %d %x\n", unsig, unsig, (int8_t) unsig, (int8_t) unsig);

    int8_t sig = -127;
    print("%d %x %u %x\n", sig, sig, (uint8_t) sig, (uint8_t) sig);

    print("%u %x %d %x\n", (uint8_t)((int8_t) unsig), (uint8_t)((int8_t) unsig), (int8_t)((uint8_t) sig), (int8_t)((uint8_t) sig));


    uint16_t unsig2 = 64000;
    print("%u %x %d %x\n", unsig2, unsig2, (int16_t) unsig2, (int16_t) unsig2);

    int16_t sig2 = -1536;
    print("%d %x %u %x\n", sig2, sig2, (uint16_t) sig2, (uint16_t) sig2);

    print("%u %x %d %x\n", (uint16_t)((int16_t) unsig2), (uint16_t)((int16_t) unsig2), (int16_t)((uint16_t) sig2), (int16_t)((uint16_t) sig2));

    print("\n");
}
