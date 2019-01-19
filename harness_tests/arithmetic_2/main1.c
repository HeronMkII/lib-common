/*
Test harness code to test greater than, less than and not equal assertions
*/

#include <test/test.h>
#include <uart/uart.h>

#define F_CPU 8000000UL
#include <util/delay.h>

void not_equal_test(void) {
    ASSERT_NEQ(2, 1); //Assert true
    ASSERT_NEQ(1, 1); //Assert false
}
void greater_test(void) {
    ASSERT_GREATER(2, 1); //Assert true
    ASSERT_GREATER(1, 20); //Assert false
}

void less_test(void) {
    ASSERT_LESS(1, 2); //Assert true
    ASSERT_LESS(20, 1); //Assert false
}

test_t t1 = { .name = "not equal assert test", .fn = not_equal_test };
test_t t4 = { .name = "greater than assert test", .fn = greater_test };
test_t t6 = { .name = "less than assert test", .fn = less_test };


test_t* suite[3] = { &t1, &t2, &t3 };

int main() {
    run_tests(suite, 3);
    return 0;
}
