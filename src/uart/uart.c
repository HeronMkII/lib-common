#include <uart/uart.h>
#include <uart/log.h>

#define MAX_RX_BUF_SIZE 50

uint8_t uart_rx_buf[MAX_RX_BUF_SIZE];
volatile uint8_t uart_rx_buf_counter;

// default rx callback
void _nop(uint8_t *c, uint8_t len) {}

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

    clear_rx_buffer();
    // reset RX buffer and counter
}

void send_uart(const uint8_t* msg, int len) {
    for (uint8_t i = 0; i < len; i++) {
        put_char(msg[i]);
    }
}

void register_callback(global_rx_cb_t cb) {
    global_rx_cb = cb;
}

void clear_rx_buffer() {
    uart_rx_buf_counter = 0;
    for (uint8_t i = 0; i < MAX_RX_BUF_SIZE; i++) {
        uart_rx_buf[i] = 0;
    }
}

ISR(LIN_TC_vect) {
  if (LINSIR & _BV(LRXOK)) {
    uint8_t c = get_char();

    uart_rx_buf[uart_rx_buf_counter] = c;
    uart_rx_buf_counter += 1;

    global_rx_cb(uart_rx_buf, uart_rx_buf_counter);

    if (uart_rx_buf_counter == MAX_RX_BUF_SIZE) {
        clear_rx_buffer();
    }
  }
}
