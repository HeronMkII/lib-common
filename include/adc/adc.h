#ifndef ADC_H
#define ADC_H

#include <avr/io.h>
#include <stdint.h>
#include <spi/spi.h>

// Number channels in the ADC
#define ADC_CHANNELS 12

//
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

uint16_t send_adc_frame(adc_t*, uint16_t);
void reset_adc(adc_t* adc);
void init_adc(adc_t*);
void fetch_all_adc_channels(adc_t*);
void fetch_channel(adc_t*, uint8_t);

uint16_t read_channel(adc_t*, uint8_t);

#endif // ADC_H
