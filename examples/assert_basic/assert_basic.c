#include <uart/uart.h>
#include <assert/assert.h>

int main(void) {
    init_uart();

    ASSERT(2 + 2 == 4);
    ASSERT(2 + 2 == 3);
    ASSERT(1 == 1.0);

    assert_print_results();
}
