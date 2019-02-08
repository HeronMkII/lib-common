/*
Test harness code to test greater than, less than and not equal assertions.
Also test floats harness code
*/

#include <test/test.h>
#include <uart/uart.h>

#define F_CPU 8000000UL
#include <util/delay.h>

void not_equal(void) {
    ASSERT_NEQ(2, 1); //Assert true
    ASSERT_NEQ(1, 1); //Assert false
}
void greater(void) { // True if first value greater than second
    ASSERT_GREATER(2, 1); //Assert true
    ASSERT_GREATER(1, 20); //Assert false
}
void less(void) { //True if first value less than second
    ASSERT_LESS(1, 2); //Assert true
    ASSERT_LESS(20, 1); //Assert false
}
// Below are assertions for float values
void float_equal(void) {
    ASSERT_FP_EQ(1.121, 1.121); //Assert true
    ASSERT_FP_EQ(-1.234, -1.2341); //Assert true due to 3 decimal truncation
    ASSERT_FP_EQ(1.876, 1.234); //Assert false
}
void float_not_equal(void) {
    ASSERT_FP_NEQ(2.946, 2.123); //Assert true
    ASSERT_FP_NEQ(10.987, 10.9876); //Assert true due to rounding
    ASSERT_FP_NEQ(10.987, 10.987); //Assert false
    ASSERT_FP_NEQ(10.23, 10.23); //Assert false
}
void float_greater(void) {
    ASSERT_FP_GREATER(2.117, 2.116); //Assert true
    ASSERT_FP_GREATER(1.221, 1.225); //Assert false
}
void float_less(void) {
    ASSERT_FP_LESS(1.155, 1.160); //Assert true
    ASSERT_FP_LESS(1.111, 1.110); //Assert false
}

void assert_pass(void) { //Assert pass
    ASSERT_FP_EQ(0, 0);
    ASSERT_FP_NEQ(1 ,2.001);
    ASSERT_FP_GREATER(1.001, 1.000);
    ASSERT_FP_LESS(0.05, 0.09);
}

test_t t1 = { .name = "not equal assert test", .fn = not_equal };
test_t t2 = { .name = "greater than assert test", .fn = greater };
test_t t3 = { .name = "less than assert test", .fn = less };
test_t t4 = { .name = "float equal assert test", .fn = float_equal };
test_t t5 = { .name = "float not equal assert test", .fn = float_not_equal };
test_t t6 = { .name = "float greater than assert test", .fn = float_greater };
test_t t7 = { .name = "float less than assert test", .fn = float_less };
test_t t8 = { .name = "float assert pass", .fn = assert_pass };

test_t* suite[8] = { &t1, &t2, &t3, &t4, &t5, &t6, &t7, &t8 };

int main() {
    run_tests(suite, 8);
    return 0;
}
