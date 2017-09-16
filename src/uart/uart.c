#include <uart/uart.h>
#include <uart/log.h>

// default rx callback
void _nop(uint8_t c) {}

void put_char(const uint8_t c) {
    int TIMEOUT = 65535;
    while ((LINSIR & _BV(LBUSY)) && TIMEOUT--);
    LINDAT = c;
}

uint8_t get_char(){
    int TIMEOUT = 65535;
    while ((LINSIR & _BV(LBUSY)) && TIMEOUT--);
    return LINDAT;
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

    LINENIR = _BV(LENRXOK);
    global_rx_cb = _nop;
    sei();
    // enable UART interrupts
}

void send_uart(const uint8_t* msg) {
    for (int i = 0; i < strlen(msg); i++) {
        put_char(msg[i]);
    }
}


void register_callback(global_rx_cb_t cb) {
    global_rx_cb = cb;
}

ISR(LIN_TC_vect) {
  if (LINSIR & _BV(LRXOK)) {
    uint8_t c = get_char();
    global_rx_cb(c);
  }
}
