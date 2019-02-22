#ifndef ADC_H
#define ADC_H

#include <avr/io.h>
#include <stdint.h>
#include <spi/spi.h>

// Number channels in the ADC
#define ADC_CHANNELS 12

// ADC operational modes
typedef enum {
    MANUAL,
    AUTO1
} adc_mode_t;

// ADC type
typedef struct {
    //auto channels
    uint16_t auto_channels;

    //not sure what this is
    pin_info_t* cs;

    // private
    //MANUAL or AUTO1 mode
    adc_mode_t mode;
    //channel data: stores the high/low reading after being fetched.
    uint16_t channel_data[ADC_CHANNELS];
} adc_t;

uint16_t send_adc_frame(adc_t* adc, uint16_t frame);
void init_adc(adc_t* adc);
void reset_adc(adc_t* adc);
void fetch_all_adc_channels(adc_t* adc);
void fetch_adc_channel(adc_t* adc, uint8_t channel);
uint16_t read_adc_channel(adc_t* adc, uint8_t channel);

#endif // ADC_H
