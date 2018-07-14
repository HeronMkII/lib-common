#ifndef DAC_H
#define DAC_H

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
} dac_t;

void init_dac(dac_t*);
void reset_dac(dac_t*);

void dac_set_voltage(dac_t*, double, dac_chan_t);

#endif
