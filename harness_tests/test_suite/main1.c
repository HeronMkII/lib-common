#include <test/test.h>
#include <uart/uart.h>

#define TEST_NO 4

void fun1(void);
void fun2(void);
void fun3(void);
void fun4(void);

test_t t1 = { .name = "Test 1", .fn = fun1, .time_min = 0.05, .time_max = 0.06 }; //purposed to fail for time_max
test_t t2 = { .name = "Test 2", .fn = fun2, .time_min = 100, .time_max = 500 }; //purposed to fail for time_min
test_t t3 = { .name = "Test 3", .fn = fun3, .time_min = 0.05, .time_max = 500 }; //purposed to produce no errors
test_t t4 = { .name = "Test 4", .fn = fun4 }; //purposed to produce no errors

test_t* suite[TEST_NO] = { &t1, &t2, &t3, &t4 };

void fun1(void) {
    print("Hello Test 1!\n");
}

void fun2(void) {
    print("Hello Test 2!\n");
}

void fun3(void) {
    print("Hello Test 3!\n");
}

void fun4(void) {
    print("Hello Test 4!\n");
}

int main() {
    run_tests(suite, TEST_NO);
    return 0;
}
