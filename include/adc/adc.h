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
    uint16_t channels;
    pin_info_t* cs;

    // private
    adc_mode_t mode;
    uint16_t channel[ADC_CHANNELS];
} adc_t;


void init_adc(adc_t*);
void fetch_all(adc_t*);
void fetch_channel(adc_t*, uint8_t);

uint16_t read_channel(adc_t*, uint8_t);

#endif // ADC_H
