#include <test/test.h>
#include <uart/uart.h>

/* Test functionality of put_char */
void put_char_test(void){
    for(int i=0;i<7;i++){
        put_uart_char(_BV(i));
        ASSERT_EQ(LINDAT, _BV(i));
    }
}

/* Test functionality of get_char */
void get_char_test(void){
    uint8_t temp;
    for(int i=0;i<7;i++){
        put_uart_char(_BV(i));
        get_uart_char(&temp);
        ASSERT_EQ(temp, LINDAT);
    }
}

/* Test functionality of init_uart and verifies that registers are set as expected */
void init_uart_test(void){
    init_uart();
    ASSERT_EQ(LINBRRH, 0);
    ASSERT_EQ(LINBRRL, 25);
    // Check LBT[5:0] (p.297)
    ASSERT_EQ(LINBTR & 0x3F, 32);
    ASSERT_EQ(LINCR, _BV(LENA) | _BV(LCMD2) | _BV(LCMD1) | _BV(LCMD0));
    ASSERT_EQ(LINENIR, _BV(LENRXOK));
}



test_t t1 = { .name = "put_char test", .fn = put_char_test };
test_t t2 = { .name = "get_char test", .fn = get_char_test };
test_t t3 = { .name = "init_uart test", .fn = init_uart_test };

test_t* suite[3] = {&t1, &t2, &t3};

int main(void){
    init_uart();
    run_tests(suite, 3);
    return 0;
}
