#include "uart.h"
#include "log.h"

void put_char(const unsigned char c) {
    int TIMEOUT = 65535;
    while ((LINSIR & _BV(LBUSY)) && TIMEOUT--);
    LINDAT = c;
}

char get_char(){
  while (LINSIR & _BV(LBUSY));
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

    // enable UART interrupts
    LINENIR = _BV(LENRXOK);
    sei();
}

void send_uart(const unsigned char* msg) {
    for (int i = 0; i < strlen(msg); i++) {
        put_char(msg[i]);
    }
}

// RX interrupt handler
ISR(LIN_TC_vect){
  if(LINSIR & _BV(LRXOK)){
    char val = get_char();
    if(val>10) serial_handler(val);
  }
}

// Change this function to do something, such as a switch statement
// Maybe a certain input will indicate a series of upcoming integers
serial_handler(char c){
  put_char(c);
}
