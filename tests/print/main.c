#include <test/test.h>
#include <uart/uart.h>

void literal_test(void) {
    print("The quick brown fox jumped over the lazy dog.\n");
}

void fmt_test(void) {
    for (uint8_t i = 0; i < 10; i++) {
        print("%d ", i);
    }
    print("\n");
}

void pretty_fmt_test(void) {
    for (uint8_t i = 1; i < 16; i++) {
        print("%#.2x ", i);
    }
    print("\n");
}

void debug_test(void) {
    print("%s %d\n", __FUNCTION__, __LINE__);
}

test_t t1 = { .name = "Print a string literal", .fn = literal_test };
test_t t2 = { .name = "Print w/ formatting", .fn = fmt_test };
test_t t3 = { .name = "Print w/ fancy formatting", .fn = pretty_fmt_test };
test_t t4 = { .name = "Print debug info", .fn = debug_test };

test_t* suite[4] = { &t1, &t2, &t3, &t4 };

int main() {
    run_tests(suite, 4);
    return 0;
}
