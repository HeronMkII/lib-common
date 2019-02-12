/*
DAC7562 - DAC (Digital to Analog Converter)

A DAC is a device that takes a digital signal (sequence of bits over SPI) as
input and produces an analog voltage on a pin as output.

This is used in PAY to control the analog hardware involving comparators for
heater control. This is used in PAY-Optical to set a reference voltage for the
ADC (Analog to Digital Converter) that converts optical sensor measurements.

Datasheet: http://www.ti.com/lit/ds/symlink/dac8162.pdf
Important pages:
- p.1 - intro
- p.4 - pin arrangement/descriptions
- p.28 - voltage conversion formula
- p.28-35 - operation, settings
- p.36-38 - programming, registers, SPI
- **p.38 - Table 17** - all SPI commands

Table 16 is somewhat misleading - only applies to writing register values for
    voltages

Every SPI communication is 24 bits, with some bits being ignored as don't care
    bits.

- internal voltage reference - VREF = 2.5V, gain = 2
- LDAC disabled/inactive - connected to GND in hardware
    - this is optional functionality to update both voltage outputs simultaneously
- it appears that the DAC cannot output a voltage higher than AVDD (about 2.9V in testing)
- "update" means updating the pin output voltage to match its register
- Not using power down modes
*/

#include <dac/dac.h>

// SPI commands (pg 38)
// "Enable internal reference and reset DACs to gain = 2"
#define ENABLE_INT_REF  ((0b111UL << 19) | 0b1)
// "LDAC pin inactive for DAC-B and DAC-A"
#define DISABLE_LDAC    ((0b110UL << 19) | 0b11)


// Sends a 24 bit DAC command over SPI
// {xx, C2-0, A2-0, DB15-0} - see Table 17
void send_dac_spi(dac_t* dac, uint32_t data) {
    uint8_t spi1 = (data >> 16) & 0xFF;
    uint8_t spi2 = (data >> 8) & 0xFF;
    uint8_t spi3 = data & 0xFF;

    // Need to set CPHA = 1 since the DAC samples data on the falling edge of
    // SCLK (p. 4) (inferred as mode 1)
    set_spi_mode(1);

    set_cs_low(dac->cs->pin, dac->cs->port);
    send_spi(spi1);
    send_spi(spi2);
    send_spi(spi3);
    set_cs_high(dac->cs->pin, dac->cs->port);

    // Restore old SPI mode
    reset_spi_mode();
}

// Initializes the DAC for use
void init_dac(dac_t* dac) {
    // Initialize CS pin, high by default
    init_cs(dac->cs->pin, dac->cs->ddr);

    // Initialize CLR pin, high by default
    init_output_pin(dac->clr->pin, dac->clr->ddr, 1);

    // Enable internal voltage reference
    send_dac_spi(dac, ENABLE_INT_REF);
    // Disable LDAC
    send_dac_spi(dac, DISABLE_LDAC);

    // reset dac, set raw voltages to 0
    reset_dac(dac);
}

// Resets the DAC
// pg 4, sets output to zero scale
// pg 34, write to register immediately outputs to zero scale
void reset_dac(dac_t* dac) {
    set_pin_low(dac->clr->pin, dac->clr->port);
    _delay_ms(1);
    set_pin_high(dac->clr->pin, dac->clr->port);

    dac->raw_voltage_a = 0;
    dac->raw_voltage_b = 0;
}

// Sets the output voltage for the specified output pin (C)
// 12bits = equivalent 12 bit value of output voltage
void set_dac_raw_voltage(dac_t* dac, dac_chan_t channel, uint16_t raw_data) {
     // "Write to DAC-C input register and update DAC-Channel"  (Table 17)
    uint32_t spi = (0b011UL << 19) | (((uint32_t) channel) << 16) |
        (raw_data << 4);
    send_dac_spi(dac, spi);

    switch (channel) {
        case DAC_A:
            dac->raw_voltage_a = raw_data;
            break;

        case DAC_B:
            dac->raw_voltage_b = raw_data;
            break;
    }
}

// Sets the output voltage for the specified output pin (C)
// voltage - in V (after gain, as measured for hardware output)
void set_dac_voltage(dac_t* dac, dac_chan_t channel, double voltage) {
    uint16_t raw_data = dac_vol_to_raw_data(voltage);
    set_dac_raw_voltage(dac, channel, raw_data);
}
