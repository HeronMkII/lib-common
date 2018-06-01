#include <avr/io.h>
#include <stdint.h>

typedef void(*test_fn_t)(void);

typedef struct {
    char name[30];
    test_fn_t fn;
} test_t;

void run_tests(test_t**, uint8_t);
