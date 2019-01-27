#include <test/test.h>
#include <uart/uart.h>
#include <adc/adc.h>

//adc object to test
uint16_t channels = ;//uhhh how are channels made..
pin_info_t* cs = {};//uhh
adc_mode_t mode;
uint16_t channel[ADC_CHANNELS] = {};//uhhhh

adc_t* adc = {channels, cs, mode, channel};
//okie so creating the adc to test is proving
//a challenge.. please check over

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
