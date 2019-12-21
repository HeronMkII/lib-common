#include <adc/adc.h>
#include <conversions/conversions.h>
#include <uart/uart.h>

// Uncomment to use EPS configuration (16 channels)
// pin_info_t cs = {
//     .port = &PORTB,
//     .ddr = &DDRB,
//     .pin = PB2
// };
// uint8_t num_channels = 16;

// Uncomment to use PAY (ADC1) configuration  (12 channels)
pin_info_t cs = {
    .port = &PORTB,
    .ddr = &DDRB,
    .pin = PB5
};
uint8_t num_channels = 12;

// Uncomment to use PAY (ADC2) configuration  (12 channels)
// pin_info_t cs = {
//     .port = &PORTB,
//     .ddr = &DDRB,
//     .pin = PB6
// };
// uint8_t num_channels = 12;

adc_t adc = {
    .auto_channels = 0x0300, // poll pins 8 and 9 of the ADC in auto-1 mode
    .cs = &cs
};

void print_voltage(adc_t* adc, uint8_t c) {
    uint16_t raw = read_adc_channel(adc, c);
    double ch_vol = adc_raw_to_ch_vol(raw);
    print("Channel: %u, Raw Data: 0x%.3x, Channel Voltage: %.6f V\n",
            c, raw, ch_vol);
}

// This test reads the raw data and voltages on each ADC channel
int main(void) {
    init_uart();
    print("\n\nUART initialized\n");

    init_adc(&adc);
    print("ADC initialized\n");

    while (1) {
        // Fetch individual channels; using MANUAL mode under the hood
        // This ends up using ~3 frames per fetch; thus 36 frames total.
        for (uint8_t i = 0; i < num_channels; i++) {
            // print("manual fetch %u\n", i);
            fetch_adc_channel(&adc, i);
        }
        print("\nManual mode:\n");
        for (uint8_t i = 0; i < num_channels; i++) {
            print_voltage(&adc, i);
        }
        _delay_ms(5000);

        // Fetch all channels at once; using AUTO1 mode under the hood
        // Each fetch takes only 1 frame, so this uses 2 frames total!
        // If we were interested in k channels, this would use k frames.
        // fetch_all_adc_channels(&adc);
        // print("\nAuto mode:\n");
        // print_voltage(&adc, 8);
        // print_voltage(&adc, 9);
        // _delay_ms(200);

        // Swapping between manual and auto works, but could be investigated
        // further.
    }
}
