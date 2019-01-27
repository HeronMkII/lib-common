/*
ADS7952 Analog to Digital Converter

Link to Datasheet:
http://www.ti.com/lit/ds/slas605c/slas605c.pdf

Brief Overview:
An analog to digital converter is a component that takes an analog signal input
(which can take on any value within some range) and converts it to a
digital representation (either a "high" or "low" value, indicated by a "0" or "1"
bit), which in turn can be used by the 32M1 (ie. the microcontrollers recognize
only digital data, hence the need to convert analog to digital data).

Terms:
- A "frame" is defined as every sending/recieving of two bytes.
- A "channel" is simply an input. The ADC has 12 channels, thus four bits are
required to describe them. you can select one of these input channels to read from.
   - Channel 0 is the default.
   - For channel selection, there is manual mode (default on reset) or
     auto 1 mode.


How it Works:
This device operates in 16 bit (2 byte) "frames". In every frame, we (that is,
the microcontroller) can specify whether to continue the current operational
settings, or change the operation (e.g. change selected channel number).
Simultaneously in every frame, the ADC sends the microcontroller 16 bits- the
first four of which are the channel number, while the next 12 are the actual
converted data corresponding to that channel.
The input data is 12 bits describing some voltage, and the ADC takes this in and
converts it to some digital 12 bit value. If the output is to be "high", this will
look like 12 1's, while an output of "low" would look like 12 0's.

*/

#include <adc/adc.h>

/*
NOTE: The standing assumption is that the power down bit
is never set, nor are the GPIO pins used.
*/

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
// --> build test for this.
#define REQUEST_AUTO1_MODE (AUTO1_MODE | EN_PGM | RANGE2 | CHAN_CTR_RST)
#define ENTER_AUTO1_MODE (AUTO1_MODE | EN_PGM | RANGE2)
#define CONTINUE_AUTO1_MODE (AUTO1_MODE | EN_PGM | RANGE2)


/*
Sending 16 bits to and from the ADC as per SPI protocol.
'Send SPI frame' called twice in order to get the 16 bits through.
This function gets each device to send one bit of information at a time
simultaneously, until the full 16 bits are sent and recieved.

@param adc_t* adc - the ADC
@param uint16_t frame - the 16 bits of the frame
*/
uint16_t send_adc(adc_t* adc, uint16_t frame) {//ps the cs fcns are from spi... idk how much of that to note
    set_cs_low(adc->cs->pin, adc->cs->port);
    uint8_t d1 = send_spi((uint8_t)(frame >> 8));
    uint8_t d2 = send_spi((uint8_t)(frame));
    set_cs_high(adc->cs->pin, adc->cs->port);

    return ((uint16_t) d1 << 8) | d2;
}

/*
Initializing ADC:
Channel 0 is the default.
Manual mode is the default on reset for channel selection.

@param adc_t* adc - ADC to initialize.
*/
void init_adc(adc_t* adc) {
    // Intialize SPI
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
    //(in response to this...)
    //idea: power down then restart?
    //nikoo has proposal: set DI05 = 1 to power down, then immediately DI05 = 0 to restart.
    //adc->channel[i] indicates each of the 12 channels, correct?
    //therefore a reset could look like this:
    //  adc->channel[5] = 1; //powers down on SCLK falling edge (idk what that is but ok)
    //  adc->channel[5] = 0;
    //(verify this is correct...)
    //(based on table p.33 of datasheet)

    //new TODO: come up with test to verify this procedure to reset the ADC...
    // need i only check the channel = 0?


    // Program auto-1 register
    uint16_t f1 = PGM_AUTO1_REG;//cmd of changing mode to auto-mode- where it fetches all channels
    uint16_t f2 = adc->channels;
    send_adc(adc, f1);
    send_adc(adc, f2);
}

/*
Updates all the values in each channel that are read by the ADC.

@param adc_t* adc - ADC
*/
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

/*
@param adc_t* adc - ADC
@param uint8_t c - the channel
*/
void fetch_channel(adc_t* adc, uint8_t c) {
    uint16_t channel_addr = ((uint16_t) c) << 7;
    uint16_t frame = MANUAL_MODE | EN_PGM | channel_addr | RANGE2;

    // TODO: This doesn't seem to matter
    // if (adc->mode == AUTO1) send_adc(adc, frame);
    //--> need to build test for this.

    send_adc(adc, frame);
    send_adc(adc, frame);
    adc->channel[c] = send_adc(adc, frame) & 0x0fff;

    adc->mode = MANUAL;
}

/*
@param adc_t* adc - the ADC
@param uint8_t c - the channel
*/
uint16_t read_channel(adc_t* adc, uint8_t c) {
    return adc->channel[c];
}
