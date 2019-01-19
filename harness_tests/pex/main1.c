#include <test/test.h>
#include <pex/pex.h>

/*
    The goals of this harness test are:
    1. 
*/

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

/* Test that the pex has been correctly initialized */
void init_pex_test(void) {
    init_pex(&pex);
    ASSERT_EQ(read_register(&pex, PEX_IOCON_DEFAULT));
}

int main(void) {


    return 0;
}
