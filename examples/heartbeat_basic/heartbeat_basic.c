/*
    Heartbeat Protocol:
        (1) In SSM main program, input ssm_id,(OBC, PAY, EPS)
        as per line 37
        (2) Call init_heartbeat() to set up heartbeat SSM
        configuration. See documented details in heartbeat.c
        (3) Call heartbeat(), WHENEVER there is a status change
        via SSM status global variable in SSM main program.

	Authors: Brytni Richards, Jack Berezny, Bonnie Weng
*/

#include <uart/uart.h>
#include <can/can.h>
#include <heartbeat/heartbeat.h>

// Extend the visibility of SSM status global variables to SSM main program
extern uint8_t obc_status;
extern uint8_t eps_status;
extern uint8_t pay_status;

//Extend the visibility of SSM status global pointers to SSM main program
extern uint8_t* self_status;
extern uint8_t* parent_status;
extern uint8_t* child_status;

extern uint8_t receiving_id; // obc {0x00} eps {0x02} pay {0x01}
extern uint8_t fresh_start;

uint8_t ssm_id; // User define ssm_id in main program

// This main function simulates the example file in lib-common
int main() {
    init_uart();
    init_can();

    // Input SSM_ID: vital to heartbeat
    ssm_id = OBC;

    init_heartbeat();
    print("heartbeat initialized\n");
    print("obc %d, pay %d, eps %d\n", obc_status, pay_status, eps_status);

    // Simulate SSM status change as mission progresses
    obc_status += 1; // or in general: *self_status += 1;
    heartbeat();
    print("obc %d, pay %d, eps %d\n\n", obc_status, pay_status, eps_status);

    while(1); // Simulate the reset condition
    /*Enable this inifinite while loop, such that main() never ends.
    This allows testers to press the reset bottons on PCBs to observe the
    status increments. Tester will observe that SSM status will not go back
    to zero because of our DEADBEEF implementation*/
    return 0;
}
