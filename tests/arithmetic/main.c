#include <test/test.h>
#include <uart/uart.h>

void addition_test(void) {
    assert_eq(1 + 1, 2);
}

test_t t1 = { .name = "Add 1 and 1", .fn = addition_test };

test_t* suite[1] = { &t1 };

int main() {
    run_tests(suite, 1);
    return 0;
}

