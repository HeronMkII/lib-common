#include <uart/uart.h>
#include <uart/log.h>
#include <can/can.h>

void tx_callback(uint8_t*, uint8_t*);

mob_t tx_mob = {
    .mob_num = 0,
    .mob_type = TX_MOB,
    .dlc = 7,
    .id_tag = { 0x0000 },
    .id_mask = { 0x0000 },
    .ctrl = {
        .rtr = 1
    },
    .tx_data_cb = tx_callback,
};

// remote frames have no data
void tx_callback(uint8_t* data, uint8_t* len) {
  *len = 0;
}

int main(void) {
  init_uart();
  print("UART initialized\n");

  init_can();
  init_tx_mob(&tx_mob);

  resume_mob(&tx_mob);

  while (1) {}
}
