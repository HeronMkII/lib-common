#include <test/test.h>
#include <uart/uart.h>

// TODO: When the input types are changed to uint32_t, the arithmetic
// tests fail! Investigate why this happens.
void assert_eq(uint8_t a, uint8_t b) {
    print("ASSERT EQ %d %d\r\n", a, b);
}
