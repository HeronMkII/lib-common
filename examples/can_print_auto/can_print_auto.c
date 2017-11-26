#include <uart/uart.h>
#include <uart/log.h>
#include <can/can.h>

void tx_callback(uint8_t*, uint8_t*);

mob_t auto_mob = {
    .mob_num = 0,
    .mob_type = AUTO_MOB,
    .dlc = 7,
    .id_tag = { 0x0000 },
    .id_mask = { 0x0000 },
    .ctrl = {
        .rtr = 1,
        .rplv = 1
    },
    .tx_data_cb = tx_callback,
};

// on a remote frame, expect to send back a "hello!"
void tx_callback(uint8_t* data, uint8_t* len) {
  *len = 7;
  char str[] = "Hello!";

  for(uint8_t i = 0; i < *len; i++) {
      data[i] = str[i];
  }
  print("%s\n",data);
}

int main(void) {
  init_uart();
  print("UART initialized\n");

  init_can();
  init_auto_mob(&auto_mob);

  resume_mob(&auto_mob);

  while (1) {}
}
