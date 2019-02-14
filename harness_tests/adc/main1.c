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
    .auto_channels = 0x0c00, // poll pins 10 and 11 of the ADC in auto-1 mode
    .cs = &cs
};

void init_adc_test(void){
    init_adc(&adc);

    //check all auto_channels are 0 to start
    for (uint8_t i = 0; i < ADC_CHANNELS; i++){
    ASSERT_EQ(adc.channel_data[i], 0);
    }
    //check the mode is manual
    ASSERT_EQ(adc.mode, MANUAL);

}

//test fetch before reset
void fetch_channel_test(void){
    uint16_t prev = adc.channel_data[11];

    fetch_channel(&adc, 11);

    ASSERT_EQ(adc.mode, MANUAL);
    ASSERT_NEQ(prev, adc.channel_data[11]);
    //is it correct to assume the value must have changed? check/confirm that fact.
}

void fetch_all_test(void){
    uint16_t *prev[ADC_CHANNELS];
    for (uint8_t i = 0; i < ADC_CHANNELS; i++){
        prev[i] = adc.channel_data[i];
    }

    fetch_all_adc_channels(&adc);

    ASSERT_EQ(adc.mode, AUTO1);

    for (uint8_t i = 0; i < ADC_CHANNELS; i++){
            if (i == 10 || i== 11){
                ASSERT_NEQ(prev[i], adc.channel_data[i]);
            } else {
                ASSERT_EQ(prev[i], adc.channel_data[i]);
            }
    }
    //once again, check it's ok to assume the values must have changed.
}

//test this before init
void reset_adc_test(void){
    //channel 5, reset, check for channel 0
    fetch_channel(&adc,5); //reading from channel 5
    read_channel(&adc,5);

    uint16_t test = send_adc_frame(&adc,0x0000);
    ASSERT_EQ(test & 0xf000, 0x5000);
    reset_adc(&adc);
    //so now im back to 0

    test = send_adc_frame(&adc,0x0000);

    ASSERT_EQ(test & 0xf000, 0x0000);
    //first four bits should be zero, while idk what the rest are

    ASSERT_EQ(adc.mode, MANUAL);
}


test_t t1 = { .name = "init_adc_test", .fn = init_adc_test };
test_t t2 = { .name = "fetch_channel_test", .fn = fetch_channel_test };
test_t t3 = { .name = "fetch_all_test", .fn = fetch_all_test };
test_t t4 = { .name = "reset_adc_test", .fn = reset_adc_test };

test_t* suite[4] = { &t1, &t2, &t3, &t4 };

int main(void) {
    run_tests(suite, 4);
    return 0;
}
