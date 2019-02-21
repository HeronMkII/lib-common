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


Usages:
1. Initialize ADC -> init_adc()
2. Call a fetch on a particular channel or all the channels to update the values
on the channel array -> fetch_all_adc_channels() or fetch_adc_channel(c)
where c is the channel number
3. Call read to return the value on a particular channel -> read_adc_channel(c)

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

// DI05 - power down (p.32)
#define POWER_DN 0x0020
#define POWER_UP 0x0000

// TODO: Verify that these are actually correct
// --> build test for this.
#define REQUEST_AUTO1_MODE (AUTO1_MODE | EN_PGM | RANGE2 | CHAN_CTR_RST)
#define ENTER_AUTO1_MODE (AUTO1_MODE | EN_PGM | RANGE2)
#define CONTINUE_AUTO1_MODE (AUTO1_MODE | EN_PGM | RANGE2)
#define START_RESET (MANUAL_MODE | EN_PGM | RANGE2 | POWER_DN)
#define STOP_RESET (MANUAL_MODE | EN_PGM | RANGE2 | POWER_UP)

/*
Sending 16 bits to and from the ADC as per SPI protocol.
'Send SPI frame' called twice in order to get the 16 bits through.
This function gets each device to send one bit of information at a time
simultaneously, until the full 16 bits are sent and recieved.

@param adc_t* adc - the ADC
@param uint16_t frame - the 16 bits of the frame
*/
uint16_t send_adc_frame(adc_t* adc, uint16_t frame) {
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
        adc->channel_data[i] = 0;
    }

    adc->mode = MANUAL;

    // Set GPIO port for ADC CS on the 32m1 to be output
    init_cs(adc->cs->pin, adc->cs->ddr);
    set_cs_high(adc->cs->pin, adc->cs->port);

    reset_adc(adc);


    // Program auto-1 register
    uint16_t f1 = PGM_AUTO1_REG;//cmd of changing mode to auto-mode- where it fetches all channels
    uint16_t f2 = adc->auto_channels;
    send_adc_frame(adc, f1);
    send_adc_frame(adc, f2);
}
/*
Resets the ADC: powers it off, then powers it on.
*/

void reset_adc(adc_t* adc){
    uint16_t frame = START_RESET;
    send_adc_frame(adc,frame);
    frame = STOP_RESET;
    send_adc_frame(adc,frame);
    //based on table p.33 of datasheet
}

/*
Gets the digital data from all channels in the ADC and updates
the channel array at all indicies.
@param adc_t* adc - ADC
*/
void fetch_all_adc_channels(adc_t* adc) {
    if (adc->mode == MANUAL) {
        send_adc_frame(adc, REQUEST_AUTO1_MODE);
        send_adc_frame(adc, ENTER_AUTO1_MODE);
    }

    for (uint8_t i = 0; i < ADC_CHANNELS; i++) {
        if (adc->auto_channels & _BV(i)) { //if the bits representing the channel match the auto_channels
            adc->channel_data[i] = send_adc_frame(adc, CONTINUE_AUTO1_MODE) & 0x0fff;
        }
    }
    adc->mode = AUTO1;
}

/*
Gets the digital data from one channel in the ADC and updates
channel array at index c.
@param adc_t* adc - ADC
@param uint8_t c - the specified channel
*/
void fetch_adc_channel(adc_t* adc, uint8_t channel) {
    uint16_t channel_addr = ((uint16_t) channel) << 7;
    uint16_t frame = MANUAL_MODE | EN_PGM | channel_addr | RANGE2;

    send_adc_frame(adc, frame);
    send_adc_frame(adc, frame);
    adc->channel_data[channel] = send_adc_frame(adc, frame) & 0x0fff;

    adc->mode = MANUAL;
}

/*
Reads the inforamtion currently stored in the ADC.
Returns a 16-bit unsigned integer that is stored in the channel array at index c.
@param adc_t* adc - the ADC
@param uint8_t c - the specified channel
*/
uint16_t read_adc_channel(adc_t* adc, uint8_t channel) {
    return adc->channel_data[channel];
}
