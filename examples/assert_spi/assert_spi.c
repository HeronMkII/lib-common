#include <uart/uart.h>
//#include <uart/log.h>
#include <spi/spi.h>
#include <assert/assert.h>

#define CLK PB7
#define MISO PB0
#define MOSI PB1
#define SS PD3

int main() {
  init_uart();
  // set pin 4 to high/output
  init_cs(4, &DDRB);
  // isolate 4th position in DDRB
  uint8_t pin = DDRB&(1<<4);
  //shift value to 0th position
  pin >>= 4;
  //check if it's high ie equals 1
  ASSERT(pin == 1);

  //check set cs low/for input (so slave is listening)
  set_cs_low(4,&PORTB);
  pin = PORTB&(1<<4);
  pin >>= 4;
  ASSERT(pin == 0);

  //check set cs high/for output (so not listening)
  set_cs_high(4,&PORTB);
  pin = PORTB&(1<<4);
  pin >>= 4;
  ASSERT(pin == 1);

  //check init_spi
  init_spi();
  //check MOSI and CLK are output (ie high)
  uint8_t pins = DDRB & (_BV(MOSI) + _BV(CLK));
  ASSERT(pins == (_BV(MOSI) + _BV(CLK)));
  //check SS is output (ie high)
  uint8_t SS_pin = DDRD & _BV(SS);
  ASSERT(SS_pin == _BV(SS));
  //check
  pins = SPCR & (_BV(SPE) + _BV(MSTR) + _BV(SPR1));
  ASSERT(pins == (_BV(SPE) + _BV(MSTR) + _BV(SPR1)));

  //check send SPI
  //send_spi(0x00);

  //print results of assert test
  assert_print_results();
  return 0;
}
