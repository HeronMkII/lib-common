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

void init_adc_test(void){
  init_adc()

  //check all channels are 0 to start
  for (uint8_t i = 0; i < ADC_CHANNELS; i++){
    ASSERT_EQ(adc->channel[i], 0);
  }
  //check the mode is manual
  ASSERT_EQ(adc->mode, MANUAL);

  //note: im assuming i neednt check init_cs, set_cs_high
  //since they are in spi, which has a harness test.
  //--plz verify this point.

  //TODO: figure out if there's any other check necessary
  //to ensure ADC is reset.
}

//uhhhhhhhhhh



int main() {
    //uhh...
    return 0;
}
