/*This is essentially the draft of assert test file*/
#include <uart/uart.h>
#include <can/can.h>
//#include <util/delay.h>
#include <uart/log.h>
#include <heartbeat/heartbeat.h>

//#define F_CPU 8

//initial definitions of heartbeat global variables
uint8_t ssm_id = 0b11;
uint8_t obc_status = 0x00; //global variables to store SSM status
uint8_t eps_status = 0x00;
uint8_t pay_status = 0x00;

int main() {
    ssm_id = 0b00;
    init_heartbeat();
}
