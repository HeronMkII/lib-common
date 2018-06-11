#include <uart/uart.h>
#include <can/can.h>

void tx_callback(uint8_t*, uint8_t*);
void rx_callback(const uint8_t*, uint8_t);

mob_t tx_mob = {
    .mob_num = 0,
    .mob_type = TX_MOB,
    .dlc = 6,
    .id_tag = { 0x0001 },
    .id_mask = { 0x0000 },
    .ctrl = {
        .rtr = 1
    },
    .tx_data_cb = tx_callback,
    .rx_cb = rx_callback
};

/*
    Interesting note: when the remote frame is not serviced, this calls
    the TX callback three times; looks like a bug
*/

// remote frames have no data
void tx_callback(uint8_t* data, uint8_t* len) {
    static uint8_t k = 0;

    if (k < 3) {
        *len = 6;
    } else {
        *len = 0;
    }
    k += 1;

    print("Remote frame sent\n");
    print("Waiting for data frame\n");
}

/*
   When a TX MOb sends a dataframe (i.e. the RTR bit is set)
   CAN automatically transforms this MOb into an RX mob, and when a reply
   is received, the RXOK flag is set. Thus TX MObs sending remote frames
   must have the rx_cb set.
*/

void rx_callback(const uint8_t* data, uint8_t len) {
    print("Data frame received!\n");
    print("%s\n", (char*) data);
}

int main(void) {
  init_uart();
  print("UART initialized\n");

  init_can();
  init_tx_mob(&tx_mob);

  resume_mob(&tx_mob);

  while (1) {}
}
