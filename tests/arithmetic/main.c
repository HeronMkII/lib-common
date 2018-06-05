#include <test/test.h>
#include <uart/uart.h>

void addition_test(void) {
    ASSERT_EQ(1 + 1, 2);
}

void one_test(void) {
    ASSERT_TRUE(1);
}

test_t t1 = { .name = "Add 1 and 1", .fn = addition_test };
test_t t2 = { .name = "Assert true", .fn = one_test };

test_t* suite[2] = { &t1, &t2 };

int main() {
    run_tests(suite, 2);
    return 0;
}
