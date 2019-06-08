#include <test/test.h>
#include <uart/uart.h>

#define TEST_NO 2

void fun1(void);
void fun2(void);

test_t t1 = { .name = "Test 1", .fn = fun1, .time_min = 1, .time_max = 5 }; //harness.py will extract this from console logs
test_t t2 = { .name = "Test 2", .fn = fun2 };

test_t* suite[TEST_NO] = { &t1, &t2 };

void fun1(void) {
    print("Hello Test 1!\n");
}

void fun2(void) {
    print("Hello Test 2!\n");
}

int main() {
    run_tests(suite, TEST_NO);
    return 0;
}
