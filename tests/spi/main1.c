#include <test/test.h>
#include <uart/uart.h>
#include <spi/spi.h>

#define CLK PB7
#define MISO PB0
#define MOSI PB1
#define SS PD3

void init_cs_test(void) {
  //init_uart();
  // set pin 4 to high/output
  init_cs(4, &DDRB);

  //ASSERT_TRUE(0x10 & var)
  // isolate 4th position in DDRB
  uint8_t pin = DDRB&(1<<4);
  //shift value to 0th position
  pin >>= 4;
  //check if it's high ie equals 1
  ASSERT_EQ(pin,1);
// or ASSERT_TRUE(pin); means it can be anything but 0 so use ASSERT_EQ because wants it to be just one
}

void set_cs_low_test(void) {
  //check set cs low/for input (so slave is listening)
  set_cs_low(4,&PORTB);
  uint8_t pin = PORTB&(1<<4);
  pin >>= 4;
  ASSERT_EQ(pin,0);
}

void set_cs_high_test(void) {
  //check set cs high/for output (so not listening)
  set_cs_high(4,&PORTB);
  uint8_t pin = PORTB&(1<<4);
  pin >>= 4;
  ASSERT_EQ(pin,1);
}

void init_spi_test_1(void) {
  //check init_spi
//  init_spi();
  //check MOSI and CLK are output (ie high)
  uint8_t pins = DDRB & (_BV(MOSI) + _BV(CLK));
  ASSERT_EQ(pins,(_BV(MOSI) + _BV(CLK)));
}
void init_spi_test_2(void) {
//  init_spi();
  //check SS is output (ie high)
  uint8_t SS_pin = DDRD & _BV(SS);
  ASSERT_EQ(SS_pin,_BV(SS));
}
void init_spi_test_3(void) {
//  init_spi();
  //check
  uint8_t pins = SPCR & (_BV(SPE) + _BV(MSTR) + _BV(SPR1));
  ASSERT_EQ(pins,(_BV(SPE) + _BV(MSTR) + _BV(SPR1)));
}
  //check send SPI
  //send_spi(0x00);

  //print results of assert test

//  assert_print_results();
//  return 0;
// }

test_t t1 = { .name = "Assert init_cs", .fn = init_cs_test };
test_t t2 = { .name = "Assert set_cs_low", .fn = set_cs_low_test};
test_t t3 = { .name = "Assert set_cs_high", .fn = set_cs_high_test};
test_t t4 = { .name = "init_spi one", .fn = init_spi_test_1};
test_t t5 = { .name = "init_spi two", .fn = init_spi_test_2};
test_t t6 = { .name = "init_spi three", .fn = init_spi_test_3};

test_t* suite[6] = { &t1, &t2, &t3, &t4, &t5, &t6};

int main() {
  init_uart();
  init_spi();
  run_tests(suite, 6);
  return 0;
}
