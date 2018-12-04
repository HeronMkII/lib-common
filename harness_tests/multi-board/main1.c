#include <test/test.h>
#include <uart/uart.h>

void func() {
    print("From master\n");
}

test_t t1 = { .name = "Example test", .fn = func };

test_t* suite[1] = { &t1 };

int main() {
    run_tests(suite, 1);
    return 0;
}
