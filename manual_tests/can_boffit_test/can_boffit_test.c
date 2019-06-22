/*
CAN Error Modes Test

Authors: Jack Berezny, Andy Xie

Note: Board 2 can be inactive (i.e. not connected to CAN bus), as it can be more straightforward to run with "no ack" errors
than errors via setting bits.
Note: In order for this test to run successfully, CANREC and CANTEC must be reset to zero before initialization.
This can be accomplished by running other code (e.g. can_print_rx, tx), as these registers are read-only and cannot be set directly.
*/

#include <stdio.h>
#include <stdlib.h>
#include <uart/uart.h>
#include <can/can.h>

void tx_callback(uint8_t*, uint8_t*);
void rx_callback(const uint8_t*, uint8_t);
void error_active_test(void);
void error_passive_test_tx(void);
void bus_off_test(void);
void create_tx_error(void);
void create_rx_error(void);

mob_t tx_mob = {
    .mob_num = 0,
    .mob_type = TX_MOB,
    .id_tag = { 0x0000 },
    .ctrl = default_tx_ctrl,
    .tx_data_cb = tx_callback
};

mob_t rx_mob = {
    .mob_num = 1,
    .mob_type = RX_MOB,
    .dlc = 7, //alternatively set this to 9 and get errors that way
    .id_tag = { 0x0000 },
    .id_mask = { 0x0000 },
    .ctrl = default_rx_ctrl,
    .rx_cb = rx_callback
};

/* Sets data for transmission */
void tx_callback(uint8_t* data, uint8_t* len) {
    *len = 7;
    char str[] = "Hello!";

    for(uint8_t i = 0; i < *len; i++) {
        data[i] = str[i];
    }
}

/* Sets rx callback */
void rx_callback(const uint8_t* data, uint8_t len) {
    print("TX received!\n");
    print("%s\n", (char *) data);
}

/* Verifies the active error CAN mode (ref. data sheet)*/
void error_active_test(void){
    // At this point, CAN should be in error active mode (default)
    // Verify that CAN is in active error mode (ERRP is low, BOFF set low)
    print("Start error active\n");
    print("ERRP: %d\n",ERRP);
    print("BOFF: %d\n",BOFF);

    if (!(CANGSTA & _BV(ERRP))){
        print("PASS: ERROR ACTIVE SUCCESS\n");
    }

    else {
        print("FAIL: ERROR ACTIVE NOT SUCCESSFUL\n");
    }
}

/* Verifies the passive error CAN mode (ref. data sheet)
   This function tests for Error Passive via tx*/
void error_passive_test_tx(void){
    print("Start error passive\n");
    while(CANTEC < 128){
        create_tx_error();
        resume_mob(&tx_mob);
        while (!is_paused(&tx_mob)) {
            //wait
        };
        print("Msg sent. Tx error count: %d\n", CANTEC);
    }
    // At this point, CAN should be in error passive mode due to TX errors
    // Verify that CAN is in passive error mode (ERRP is high, BOFF set low)

    if (CANGSTA & _BV(ERRP)){
        print("PASS: ERROR PASSIVE SUCCESS\n");
    }
    else {
        print("FAIL: ERROR PASSIVE NOT SUCCESSFUL\n");
    }
}


/* Verifies the passive error CAN mode (ref. data sheet)
   This function tests for Error Passive via rx*/


/* Verifies the bus-off CAN mode (ref. data sheet)*/
void bus_off_test(void){
    print("Start bus off\n");
    // Wait until Transmit Error Count (TEC) is greater than 255
    // Note that 255 is the max value for CANTEC, so after it incremenets
    // above it will surpass the bus off threshold
    while(CANTEC < 255){
        create_tx_error();
        resume_mob(&tx_mob);
        while (!is_paused(&tx_mob)) {};
        print("Msg sent. Rx error count: %d\n", CANREC);
        print("Msg sent. Tx error count: %d\n", CANTEC);
        create_tx_error();
    }

    // Verify that CAN is in bus-off mode(ERRP is high, BOFF set high)
    if (CANGSTA & _BV(BOFF)){
        print("PASS: BUS OFF SUCCESS\n");
    }
    else {
        print("FAIL: BUS OFF NOT SUCCESSFUL\n");
    }
}

/* NOTE: If setting the registers do not
 trigger CANTEC or CANREC, can force ack or data length errors manually */
void create_tx_error(void){
    // Force bit error (tx only)
    select_mob(0);
    CANSTMOB = CANSTMOB | _BV(BERR);
}

void create_rx_error(void){
    //force CRC error (rx only)
    select_mob(1);
    CANSTMOB = CANSTMOB | _BV(CERR);
}

int main(void){
    init_can();
    init_uart(); //This also initializes uart
    print("Starting...\n");
    init_tx_mob(&tx_mob);
    init_rx_mob(&rx_mob);
    // Set ERRP, BOFF bits low (should be this by default)
    CANGSTA &= ~(_BV(ERRP) | _BV(BOFF));

    print("TEST START\n");
    error_active_test();
    error_passive_test_tx();
    bus_off_test();
    print("TESTS COMPLETE\n");
    return 0;
}
