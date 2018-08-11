/*
DAC7562 - DAC (Digital to Analog Converter)

A DAC is a device that takes a digital signal (sequence of bits over SPI) as
input and produces an analog voltage on a pin as output.

This is used in PAY to control the analog hardware involving comparators for
heater control. This is used in PAY-Optical to set a reference voltage for the ADC
(Analog to Digital Converter) that converts optical sensor measurements.

Datasheet: http://www.ti.com/lit/ds/symlink/dac8162.pdf
Important pages:
- p.1 - intro
- p.4 - pin arrangement/descriptions
- p.28 - voltage conversion formula
- p.28-35 - operation, settings
- p.36-38 - programming, registers, SPI
- **p.38 - Table 17** - all SPI commands

Table 16 is somewhat misleading - only applies to writing register values for voltages

Every SPI communication is 24 bits, with some bits being ignored as don't care bits.

- internal voltage reference - VREF = 2.5V, gain = 2
- LDAC disabled/inactive - connected to GND in hardware
    - this is optional functionality to update both voltage outputs simultaneously
- it appears that the DAC cannot output a voltage higher than AVDD (about 2.9V in testing)
- "update" means updating the pin output voltage to match its register

TODO - power down/up DAC A/B?
*/

#include <dac/dac.h>

#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include <util/delay.h>

// Internal voltage reference (V)
#define DAC_INT_VREF        2.5
// Internal voltage reference gain
#define DAC_INT_VREF_GAIN   2
// Number of bits to represent voltage
#define DAC_N               12

// SPI commands
// "Enable internal reference and reset DACs to gain = 2"
#define ENABLE_INT_REF  ((0b111UL << 19) | 0b1)
// "LDAC pin inactive for DAC-B and DAC-A"
#define DISABLE_LDAC    ((0b110UL << 19) | 0b11)


// Sends a 24 bit DAC command over SPI
// {xx, C2-0, A2-0, DB15-0} - see Table 17
void dac_send(dac_t* dac, uint32_t data) {
    uint8_t spi1 = (data >> 16) & 0xFF;
    uint8_t spi2 = (data >> 8) & 0xFF;
    uint8_t spi3 = data & 0xFF;

    // Need to set CPHA = 1 since the DAC samples data on the falling edge of
    // SCLK
    // TODO: Add support for this in the SPI library itself
    uint8_t SPCR_old = SPCR;
    SPCR |= _BV(CPHA);

    set_cs_low(dac->cs->pin, dac->cs->port);
    send_spi(spi1);
    send_spi(spi2);
    send_spi(spi3);
    set_cs_high(dac->cs->pin, dac->cs->port);

    // Restore old SPCR value
    SPCR = SPCR_old;
}

// Initializes the DAC for use
void init_dac(dac_t* dac) {
    // TODO - use generic port commands instead of SPI
    // Initialize CS and CLR pins
    init_cs(dac->cs->pin, dac->cs->ddr);
    set_cs_high(dac->cs->pin, dac->cs->port);

    init_cs(dac->clr->pin, dac->clr->ddr);
    set_cs_high(dac->clr->pin, dac->clr->port);

    // Enable internal voltage reference
    dac_send(dac, ENABLE_INT_REF);
    // Disable LDAC
    dac_send(dac, DISABLE_LDAC);
}

// Resets the DAC
void reset_dac(dac_t* dac) {
    set_cs_low(dac->clr->pin, dac->clr->port);
    _delay_ms(1);
    set_cs_high(dac->clr->pin, dac->clr->port);
}

// Converts an output voltage value to the equivalent 12 bit value
uint16_t dac_voltage_to_12bits(double voltage) {
    // p.28 - 8.3.1
    // Vout = (Din / 2^n) x Vref x Gain
    // Din = (Vout x 2^n) / (Vref x Gain)
    double num = voltage * (1 << DAC_N);
    double denom = DAC_INT_VREF * DAC_INT_VREF_GAIN;
    uint16_t result = (uint16_t) (num / denom);

    return result;
}

// Sets the output voltage for the specified output pin (C)
// voltage - in V (after gain, as measured for hardware output)
void dac_set_voltage(dac_t* dac, double voltage, dac_chan_t C) {
    uint16_t data = dac_voltage_to_12bits(voltage);
    // "Write to DAC-C input register and update DAC-C"
    uint32_t spi = (0b011UL << 19) | (((uint32_t) C) << 16) | (data << 4);
    dac_send(dac, spi);
}
