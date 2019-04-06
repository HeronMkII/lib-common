/*
BOFFIT Test

Authors: Andy Xie

This obviously needs to be more developed. Refer to the aidsheet.
*/

#include <stdio.h>
#include <stdlib.h>
#include <uart/uart.h>
#include <can/can.h>

void tx_callback(uint8_t*, uint8_t*);

mob_t tx_mob = { //We need the TX mobs for as communication for CAN needs them.
    .mob_num = 0,
    .mob_type = TX_MOB,
    .id_tag = { 0x0000 },
    .ctrl = default_tx_ctrl,
    .tx_data_cb = tx_callback
};

void tx_callback(uint8_t* data, uint8_t* len) {
    *len = 7;
    char str[] = "Hello!";

    for(uint8_t i = 0; i < *len; i++) {
        data[i] = str[i];
    }
}

void rx_callback(const uint8_t*, uint8_t); // The above statement goes for RX as well.

mob_t rx_mob = {
    .mob_num = 0,
    .mob_type = RX_MOB,
    .dlc = 7,
    .id_tag = { 0x0000 },
    .id_mask = { 0x0000 },
    .ctrl = default_rx_ctrl,
    .rx_cb = rx_callback
};

void rx_callback(const uint8_t* data, uint8_t len) {
    print("TX received!\n");
    print("%s\n", (char *) data);
}

void error_active(int ERRP,int BOFF){
    int counter = 0;
    for (int i = 0; i < 6; i++){ //This checks for signals between rx and tx five times.
        init_can();
        init_rx_mob(&rx_mob);
        init_tx_mob(&tx_mob);
        //Check for signal
        counter += 1;
    }
    if(counter != 5){ // Both being 1 displays an error
        ERRP = 1;
        BOFF = 1;
        "ERROR"
        break;
    }
    if(!(active_error_frame())){ // Failure if it doesn't pass active error frame.
        ERRP = 1;
        BOFF = 1;
    }
    return counter;
    return ERRP, BOFF;
}

void error_passive(){
    int counter = 0;
    for (int i = 0; i < 6; i++){
        init_can();
        init_rx_mob(&rx_mob);
        init_tx_mob(&tx_mob);
        counter += 1;
    }
    if(passive_error_frame()){ // Error if it doesn't pass passive error frame.
        ERRP = 1;
        BOFF = 0;
    }
    if (ERRP == 1){
        "FAILED";
    }
    return counter;
}

void bus_off(){
    int counter = 0;
    for (int i = 0; i < 6; i++){
        init_can();
        init_rx_mob(&rx_mob);
        init_tx_mob(&tx_mob);
        counter += 1;
    }
    if(counter == 0){ // Nothing should go through at this state.
        "PASSED";
    }
    return counter;
}

int main(void){
    init_uart();
    int ERRP = 0;
    int BOFF = 0;

}
