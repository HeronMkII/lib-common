#include <uart/uart.h>
#include <spi/spi.h>
#include <pex/pex.h>
#include <pex/pay.h>

#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include <util/delay.h>

pin_info_t cs = {
    .port = &PEX_CS_PORT_PAY,
    .ddr = &PEX_CS_DDR_PAY,
    .pin = PEX_CS_PIN_PAY
};

pin_info_t rst = {
    .port = &PEX_RST_PORT_PAY,
    .ddr = &PEX_RST_DDR_PAY,
    .pin = PEX_RST_PIN_PAY
};

pex_t pex = {
    .addr = PEX_ADDR_PAY,
    .cs = &cs,
    .rst = &rst
};

void print_dirs() {
    print("GPA Directions: %.2x\n", read_register(&pex, PEX_IODIR_A));
    print("GPB Directions: %.2x\n", read_register(&pex, PEX_IODIR_B));
}

void print_values() {
    print("GPA Values: %.2x\n", read_register(&pex, PEX_GPIO_A));
    print("GPB Values: %.2x\n", read_register(&pex, PEX_GPIO_B));
}

int main(void) {
    init_uart();
    print("\n\nUART initialized\n");

    init_spi();
    print("SPI Initialized\n");
    print("\n");

    print("Starting test\n");

    init_pex(&pex);
    print("PEX Initialized\n");

    print_dirs();
    print_values();
    print("\n");

    pex_set_pin_dir(&pex, 6, PEX_A, OUTPUT);
    print("Set GPA6 as output\n");
    pex_set_pin_dir(&pex, 2, PEX_A, INPUT);
    print("Set GPA2 as input\n");

    pex_set_pin_dir(&pex, 0, PEX_B, OUTPUT);
    print("Set GPB0 as output\n");
    pex_set_pin_dir(&pex, 7, PEX_B, INPUT);
    print("Set GPB7 as input\n");

    print_dirs();
    print_values();
    print("\n");

    pex_set_pin(&pex, 6, PEX_A, HIGH);
    print("Set GPA6 high\n");
    pex_set_pin(&pex, 0, PEX_B, HIGH);
    print("Set GPB0 high\n");

    print_dirs();
    print_values();

    print("Waiting 30 seconds\n");
    _delay_ms(30000);
    print("\n");


    pex_set_pin(&pex, 6, PEX_A, LOW);
    print("Set GPA6 low\n");
    pex_set_pin(&pex, 0, PEX_B, LOW);
    print("Set GPB0 low\n");

    print_dirs();
    print_values();

    print("Waiting 30 seconds\n");
    _delay_ms(30000);
    print("\n");

    print("Read GPA2 = %u\n", pex_get_pin(&pex, 2, PEX_A));
    print("Read GPB5 = %u\n", pex_get_pin(&pex, 5, PEX_B));

    print_dirs();
    print_values();

    print("Done\n");

    while (1) {}
}
