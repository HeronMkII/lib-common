#include <uart/uart.h>
#include <spi/spi.h>
#include <pex/pex.h>

// PAY-SSM configuration
pin_info_t cs = {
    .port = &PORTD,
    .ddr = &DDRD,
    .pin = PD5
};
pin_info_t rst = {
    .port = &PORTD,
    .ddr = &DDRD,
    .pin = PD6
};
const uint8_t addr = 1;

pex_t pex = {
    .addr = addr,
    .cs = &cs,
    .rst = &rst
};

void print_dirs() {
    print("GPA Directions: %.2x\n", read_pex_register(&pex, PEX_IODIR_A));
    print("GPB Directions: %.2x\n", read_pex_register(&pex, PEX_IODIR_B));
}

void print_values() {
    print("GPA Values: %.2x\n", read_pex_register(&pex, PEX_GPIO_A));
    print("GPB Values: %.2x\n", read_pex_register(&pex, PEX_GPIO_B));
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

    set_pex_pin_dir(&pex, PEX_A, 6, OUTPUT);
    print("Set A6 as output\n");
    set_pex_pin_dir(&pex, PEX_A, 2, INPUT);
    print("Set A2 as input\n");

    set_pex_pin_dir(&pex, PEX_B, 1, OUTPUT);
    print("Set B1 as output\n");
    set_pex_pin_dir(&pex, PEX_B, 7, INPUT);
    print("Set B7 as input\n");

    print_dirs();
    print_values();
    print("\n");

    set_pex_pin(&pex, PEX_A, 6, 1);
    print("CHECK: A6 (pin 27) high\n");
    set_pex_pin(&pex, PEX_B, 1, 0);
    print("CHECK: B1 (pin 2) low\n");

    print_dirs();
    print_values();

    print("Waiting 30 seconds\n");
    _delay_ms(30000);
    print("\n");


    set_pex_pin(&pex, PEX_A, 6, 0);
    print("CHECK: A6 (pin 27) low\n");
    set_pex_pin(&pex, PEX_B, 1, 1);
    print("CHECK: B1 (pin 2) high\n");

    print_dirs();
    print_values();

    print("Waiting 30 seconds\n");
    _delay_ms(30000);
    print("\n");

    print("Read A2 = %u\n", get_pex_pin(&pex, PEX_A, 2));
    print("Read B5 = %u\n", get_pex_pin(&pex, PEX_B, 5));

    print_dirs();
    print_values();

    print("Done\n");

    while (1) {}
}
