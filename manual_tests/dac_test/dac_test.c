// Test initializing the DAC library and setting arbitrary voltages on A and B
// The DAC is not expected to output any voltage higher than AVDD (about 2.9V)

#include <uart/uart.h>
#include <dac/dac.h>
#include <dac/pay.h>
#include <util/delay.h>

pin_info_t cs = {
    .pin = DAC_CS_PIN_PAY,
    .ddr = &DAC_CS_DDR_PAY,
    .port = &DAC_CS_PORT_PAY
};

pin_info_t clr = {
    .pin = DAC_CLR_PIN_PAY,
    .ddr = &DAC_CLR_DDR_PAY,
    .port = &DAC_CLR_PORT_PAY
};

dac_t dac = {
    .cs = &cs,
    .clr = &clr
};

// Define this function because _delay_ms() only works up to 200ms
void delay_s(uint16_t seconds) {
    print("Waiting for %u seconds\n", seconds);
    for (uint16_t i = 0; i < seconds; ++i) {
        for (uint8_t j = 0; j < 10; ++j) {
            _delay_ms(100);
        }
    }
}

// *NEED TO CHOOSE BOARD AND VOLTAGES TO USE*
int main(void) {
    init_uart();
    print("\n\nUART Initialized\n");
    init_spi();
    print("SPI Initialized\n");
    print("\n");

    init_dac(&dac);
    print("DAC Initialized\n");
    print("VOUTA = pin 1\n");
    print("VOUTB = pin 2\n");
    print("\n");

    print("check VREF (pin 10) = 2.5V\n");
    print("\n");

    dac_set_voltage(&dac, DAC_A, 0.7);
    print("Set VOUTA = 0.7 V\n");
    dac_set_voltage(&dac, DAC_B, 1.2);
    print("Set VOUTB = 1.2 V\n");
    delay_s(20);
    print("\n");

    reset_dac(&dac);
    print("Reset DAC: VOUTA = 0 V, VOUTB = 0 V\n");
    delay_s(20);
    print("\n");

    for (uint8_t i = 0; i < 40; i++) {
        dac_set_voltage(&dac, DAC_A, i * 0.1);
        print("Set VOUTA = %u * 0.1V\n", i);

        dac_set_voltage(&dac, DAC_B, (40 - 1 - i) * 0.1);
        print("Set VOUTB = %u * 0.1V\n", 40 - 1 - i);

        delay_s(1);
        print("\n");
    }

    print("Done\n");
    while (1) {}
}
