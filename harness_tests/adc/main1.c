#include <test/test.h>
#include <uart/uart.h>
#include <adc/adc.h>
#include <adc/pay.h>

pin_info_t cs = {
    .port = &ADC_CS_PORT_PAY,
    .ddr = &ADC_CS_DDR_PAY,
    .pin = ADC_CS_PIN_PAY
};

adc_t adc = {
    .channels = 0x0c00, // poll pins 10 and 11 of the ADC in auto-1 mode
    .cs = &cs
};

//any way we can test send_adc?

//test fetch before reset
void fetch_channel_test(void){
  //figure out how to test this...
  fetch_channel(&adc, 11);
  //choice of 11 based on the adc_test... not sure if this works/is sufficient
  ASSERT_EQ(adc.mode, MANUAL);
  //idk what exactly to test regarding adc.channel[c]...
}

void fetch_all_test(void){
  fetch_all(&adc);
  ASSERT_EQ(adc.mode, AUTO1);
  //what else can i check?
}

//test this before init
void reset_adc_test(void){
  //channel 5, reset, check for channel 0
  fetch_channel(&adc,5); //reading from channel 5
  read_channel(&adc,5);

  reset_adc(&adc);
  //so now im back to 0

  ASSERT_EQ(adc.channel[5],0);
  ASSERT_EQ(adc.mode, MANUAL);
  //technically i didnt change mode. meh or no meh?
}

void init_adc_test(void){
  init_adc()

  //check all channels are 0 to start
  for (uint8_t i = 0; i < ADC_CHANNELS; i++){
    ASSERT_EQ(adc.channel[i], 0);
  }
  //check the mode is manual
  ASSERT_EQ(adc.mode, MANUAL);

  //note: im assuming i neednt check init_cs, set_cs_high
  //since they are in spi, which has a harness test.
  //--plz verify this point.
}


test_t t1 = { .name = "fetch_channel_test", .fn = fetch_channel_test };
test_t t2 = { .name = "fetch_all_test", .fn = fetch_all_test };
test_t t3 = { .name = "reset_adc_test", .fn = reset_adc_test };
test_t t4 = { .name = "init_adc_test", .fn = init_adc_test };

test_t* suite[4] = { &t1, &t2, &t3, &t4 };

int main(void) {
    run_tests(suite, 4);
    return 0;
}
