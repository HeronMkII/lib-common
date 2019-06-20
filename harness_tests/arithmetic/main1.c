#include <test/test.h>
#include <uart/uart.h>

#define F_CPU 8000000UL
#include <util/delay.h>

void addition_test(void) {
    ASSERT_EQ(1 + 1, 2);
}

void one_test(void) {
    ASSERT_TRUE(1);
}

void hund_ms(void) {
    _delay_ms(100);
}

test_t t1 = { .name = "Add 1 and 1", .fn = addition_test };
test_t t2 = { .name = "Assert true", .fn = one_test };
test_t t3 = { .name = "100 ms", .fn = hund_ms };

test_t* suite[3] = { &t1, &t2, &t3 };

int main() {
    run_tests(suite, 3);
    return 0;
}
