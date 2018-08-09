/*
ADS7952 analog to digital converter
- manual mode (default on reset) or auto 1 mode
- 12 bit data readings
- channel 0 default

An analog to digital converter is a component that takes an analog signal input
and converts it to a digital representation (sequence of bits) that can be used
by the 32M1. You can select one of many input channels to read from.

The device operates in 16 bit (2 byte) "frames". In every frame, we can specify
whether to continue the current operational settings, or change the operation
(e.g. change selected channel number). Simultaneously in every frame, the ADC
sends us 16 bits, where the first 4 bits are the channel number of the
conversion data and the next 12 bits are the actual conversion data
corresponding to that channel.
*/

#include <adc/adc.h>

// NOTE: The standing assumption is that the power down bit is never set,
// nor are the GPIO pins used.

// Programming the program register
#define PGM_AUTO1_REG 0x8000
// Auto-1 program register; used to tell the ADC which channels
// to cycle through in Auto-1 mode

// DI15-12 - mode of operation
#define AUTO1_MODE    0x2000
#define MANUAL_MODE   0x1000
#define CONTINUE_MODE 0x0000

// DI11 - programming or retaining bits DI06-00
#define EN_PGM        0x0800

// DI10 - channel reset
#define CHAN_CTR_RST  0x0400

// DI06 - voltage reference range
#define RANGE2        0x0040 // 5V
#define RANGE1        0x0000 // 2.5V

// TODO: Verify that these are actually correct
#define REQUEST_AUTO1_MODE (AUTO1_MODE | EN_PGM | RANGE2 | CHAN_CTR_RST)
#define ENTER_AUTO1_MODE (AUTO1_MODE | EN_PGM | RANGE2)
#define CONTINUE_AUTO1_MODE (AUTO1_MODE | EN_PGM | RANGE2)

uint16_t send_adc(adc_t* adc, uint16_t frame) {
    set_cs_low(adc->cs->pin, adc->cs->port);
    uint8_t d1 = send_spi((uint8_t)(frame >> 8));
    uint8_t d2 = send_spi((uint8_t)(frame));
    set_cs_high(adc->cs->pin, adc->cs->port);

    return ((uint16_t) d1 << 8) | d2;
}

void init_adc(adc_t* adc) {
    init_spi();

    for (uint8_t i = 0; i < ADC_CHANNELS; i++) {
        adc->channel[i] = 0;
    }

    adc->mode = MANUAL;

    // Set GPIO port for ADC CS on the 32m1 to be output
    init_cs(adc->cs->pin, adc->cs->ddr);
    set_cs_high(adc->cs->pin, adc->cs->port);

    // TODO: Figure out how to reset the ADC. See the power down (PD) pin notes
    // in the datasheet.

    // Program auto-1 register
    uint16_t f1 = PGM_AUTO1_REG;
    uint16_t f2 = adc->channels;
    send_adc(adc, f1);
    send_adc(adc, f2);
}

void fetch_all(adc_t* adc) {
    if (adc->mode == MANUAL) {
        send_adc(adc, REQUEST_AUTO1_MODE);
        send_adc(adc, ENTER_AUTO1_MODE);
    }

    for (uint8_t i = 0; i < ADC_CHANNELS; i++) {
        if (adc->channels & _BV(i)) {
            adc->channel[i] = send_adc(adc, CONTINUE_AUTO1_MODE) & 0x0fff;
        }
    }

    adc->mode = AUTO1;
}

void fetch_channel(adc_t* adc, uint8_t c) {
    uint16_t channel_addr = ((uint16_t) c) << 7;
    uint16_t frame = MANUAL_MODE | EN_PGM | channel_addr | RANGE2;

    // TODO: This doesn't seem to matter
    // if (adc->mode == AUTO1) send_adc(adc, frame);

    send_adc(adc, frame);
    send_adc(adc, frame);
    adc->channel[c] = send_adc(adc, frame) & 0x0fff;

    adc->mode = MANUAL;
}

uint16_t read_channel(adc_t* adc, uint8_t c) {
    return adc->channel[c];
}
