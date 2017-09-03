#include "uart.h"

void put_char(const unsigned char c) {
    int TIMEOUT = 65535;
    while ((LINSIR & _BV(LBUSY)) && TIMEOUT--);
    LINDAT = c;
}

void init_uart() {
    LINCR = _BV(LSWRES); // reset UART

    int16_t LDIV = (F_IO/(BAUD_RATE*BIT_SAMPLES)) - 1;
    // this number is not necessarily integer; if the number is too far from
    // being an integer, too much error will accumulate and UART will output
    // garbage

    LINBRRH = (uint8_t) LDIV >> 8;
    LINBRRL = (uint8_t) LDIV;

    LINBTR = BIT_SAMPLES;

    LINCR = _BV(LENA) | _BV(LCMD2) | _BV(LCMD1) | _BV(LCMD0);
    // enable UART, full duplex
}

void send_uart(const uint8_t* msg) {
    int i;
    
    for (i = 0; i < strnlen(msg, 80); i++) {
        put_char(msg[i]);
    }
}
