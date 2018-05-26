#ifndef HEARTBEAT_H
#define HEARTBEAT_H

//Assume init_uart() and init_can() have been called
#include <avr/eeprom.h>
#include <stdbool.h>

#include <uart/log.h>

void assign_heartbeat_status();
void init_heartbeat();

//CAN ids of each SSMs. Follow the convention of can_ids under lib-common
#define OBC_STATUS_RX_MOB_ID { 0x001c }
#define OBC_STATUS_TX_MOB_ID { 0x000b }
#define EPS_STATUS_RX_MOB_ID { 0x001b }
#define EPS_STATUS_TX_MOB_ID { 0x000a }
#define PAY_STATUS_RX_MOB_ID { 0x001a }
#define PAY_STATUS_TX_MOB_ID { 0x000c }

//EEPROM address assignment to store status of each SSM
//Address starts from 0x0000
const uint16_t INIT_WORD_EEMEM  = 0x0000;
const uint16_t OBC_STATUS_EEMEM = 0x0004;
const uint16_t EPS_STATUS_EEMEM = 0x0008;
const uint16_t PAY_STATUS_EEMEM = 0x000c;

#define DEADBEEF 0xDEADBEEF //4 bytes

extern uint8_t obc_status; //global variables to store SSM status
extern uint8_t eps_status;
extern uint8_t pay_status;

extern uint8_t* self_status;
extern uint8_t* parent_status;
extern uint8_t* child_status;

extern uint8_t ssm_id; //will be changed by each SSM
//obc {0x00} eps {10} pay {01}

extern bool fresh_start;

#endif
