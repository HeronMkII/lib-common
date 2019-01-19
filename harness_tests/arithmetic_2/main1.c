/*
Test harness code to test greater than, less than and not equal assertions.
Also test floats harness code
*/

#include <test/test.h>
#include <uart/uart.h>

#define F_CPU 8000000UL
#include <util/delay.h>

void not_equal_test(void) {
    ASSERT_NEQ(2, 1); //Assert true
    ASSERT_NEQ(1, 1); //Assert false
}
void greater_test(void) { // True if first value greater than second
    ASSERT_GREATER(2, 1); //Assert true
    ASSERT_GREATER(1, 20); //Assert false
}
void less_test(void) { //True if first value less than second
    ASSERT_LESS(1, 2); //Assert true
    ASSERT_LESS(20, 1); //Assert false
}
// Below are assertions for float values
void float_equal_test(void) {
    ASSERT_FLOAT_EQ(1.121234, 1.121234); //Assert true
    ASSERT_FLOAT_EQ(1.111234, 1.11123416); //Assert true due to 3 decimal truncation
    ASSERT_FLOAT_EQ(1.111876, 1.222234); //Assert false
}
void float_not_equal_test(void) {
    ASSERT_FLOAT_NEQ(2.232946, 2.232123); //Assert true
    ASSERT_FLOAT_NEQ(10.121987, 10.1219876); //Assert true due to rounding
    ASSERT_FLOAT_NEQ(10.232987, 10.232987); //Assert false
    ASSERT_FLOAT_NEQ(10.23, 10.23); //Assert false
}
void float_greater_test(void) {
    ASSERT_FLOAT_GREATER(2.111117, 2.111116); //Assert true
    ASSERT_FLOAT_GREATER(1.2221, 1.2225); //Assert false
}
void float_less_test(void) {
    ASSERT_FLOAT_LESS(1.565155, 1.565160); //Assert true
    ASSERT_FLOAT_LESS(1.11111, 1.11110); //Assert false
}

test_t t1 = { .name = "not equal assert test", .fn = not_equal_test };
test_t t2 = { .name = "greater than assert test", .fn = greater_test };
test_t t3 = { .name = "less than assert test", .fn = less_test };
test_t t4 = { .name = "float equal assert test", .fn = float_equal_test };
test_t t5 = { .name = "float equal assert test", .fn = float_not_equal_test };
test_t t6 = { .name = "float greater than assert test", .fn = float_greater_test };
test_t t7 = { .name = "float less than assert test", .fn = float_less_test };


test_t* suite[7] = { &t1, &t2, &t3, &t4, &t5, &t6, &t7 };

int main() {
    run_tests(suite, 7);
    return 0;
}
