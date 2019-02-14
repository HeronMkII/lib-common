#include <uart/uart.h>
#include <adc/adc.h>
#include <adc/pay.h>

#include <conversions/conversions.h>

#ifndef F_CPU
#define F_CPU 8000000L
#endif

#include <util/delay.h>

pin_info_t cs = {
    .port = &ADC_CS_PORT_PAY,
    .ddr = &ADC_CS_DDR_PAY,
    .pin = ADC_CS_PIN_PAY
};

adc_t adc = {
    .auto_channels = 0x0c00, // poll pins 10 and 11 of the ADC in auto-1 mode
    .cs = &cs
};

void print_voltage(adc_t* adc, uint8_t c) {
    uint16_t raw_data = read_channel(adc, c);
    double raw_voltage = adc_raw_data_to_raw_vol(raw_data);
    print("Channel: %u, Raw Data: 0x%.4x, Raw Voltage: %f\n",
            c, raw_data, raw_voltage);
}

// This test reads the raw data and voltages on each ADC channel
int main(void) {
    init_uart();
    print("UART initialized\n");

    init_adc(&adc);
    print("ADC initialized\n");

    while (1) {
        // Fetch individual channels; using MANUAL mode under the hood
        // This ends up using ~3 frames per fetch; thus 6 frames total.
        fetch_channel(&adc, 10);
        fetch_channel(&adc, 11);

        print_voltage(&adc, 10);
        print_voltage(&adc, 11);
        _delay_ms(200);

        // Fetch all channels at once; using AUTO1 mode under the hood
        // Each fetch takes only 1 frame, so this uses 2 frames total!
        // If we were interested in k channels, this would use k frames.
        fetch_all_adc_channels(&adc);
        print_voltage(&adc, 10);
        print_voltage(&adc, 11);
        _delay_ms(200);

        // TODO: At the moment, swapping between manual and auto works, but this
        // needs to be investigated further.
    }
}
