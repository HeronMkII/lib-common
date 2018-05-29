//Assume init_uart() and init_can() have been called
#include <avr/eeprom.h>

//Includes in the user files
#include <uart/uart.h>
#include <can/can.h>
#include <uart/log.h>
#include <timer/timer.h>
#include <heartbeat/heartbeat.h>

extern uint8_t obc_status; //global variables to store SSM status
extern uint8_t eps_status;
extern uint8_t pay_status;

//purpose of having indirect access to status is for using error checking
extern uint8_t* self_status;
extern uint8_t* parent_status;
extern uint8_t* child_status;

//obc {0x00} eps {10} pay {01}
extern uint8_t receiving_id;

extern mob_t status_rx_mob;
extern mob_t status_tx_mob;

uint8_t ssm_id;


void heartbeat_func(){
  //status change in SSM in timed manner corresponds to mission
  obc_status += 1; //or in general: *self_status += 1;
  heartbeat();
  print("obc %d, pay %d, eps %d\n\n", obc_status, pay_status, eps_status);
}

//This main function simulates the example file in lib-common
int main() {
    init_uart();
    init_can();
    ssm_id = 0x00; //obc
    init_heartbeat();
    print("heartbeat initialized\n");
    print("obc %d, pay %d, eps %d\n", obc_status, pay_status, eps_status);

    //init_timer(1, heartbeat_func);//change state every minute

    //status change in SSM in timed manner corresponds to mission
    obc_status += 1; //or in general: *self_status += 1;
    heartbeat();
    print("obc %d, pay %d, eps %d\n\n", obc_status, pay_status, eps_status);

    while(1);
    return 0;
}
