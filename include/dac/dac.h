#ifndef DAC_H
#define DAC_H

#include <conversions/conversions.h>
#include <stdint.h>
#include <spi/spi.h>

// DAC channels (two output pins)
typedef enum {
    DAC_A = 0,
    DAC_B = 1
} dac_chan_t;

// DAC device - chip select and clear pins
typedef struct {
    pin_info_t* cs;
    pin_info_t* clr;

    uint16_t raw_voltage_a;
    uint16_t raw_voltage_b;
} dac_t;

void init_dac(dac_t*);
void reset_dac(dac_t*);

void dac_set_voltage(dac_t* dac, dac_chan_t channel, double voltage);
void dac_set_voltage_with_12bits(dac_t* dac, dac_chan_t channel, uint16_t raw_data);

#endif
