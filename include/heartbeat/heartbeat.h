#ifndef HEARTBEAT_H
#define HEARTBEAT_H

//void init_heartbeat();
//void assign_heartbeat_status();

//CAN ids of each SSMs. Follow the convention of can_ids under lib-common
#define OBC_STATUS_RX_MOB_ID 0x001c
#define OBC_STATUS_TX_MOB_ID 0x000b
#define EPS_STATUS_RX_MOB_ID 0x001b
#define EPS_STATUS_TX_MOB_ID 0x000a
#define PAY_STATUS_RX_MOB_ID 0x001a
#define PAY_STATUS_TX_MOB_ID 0x000c

#define DEADBEEF 0Xdeadbeef //4 bytes

extern uint8_t obc_status; //global variables to store SSM status
extern uint8_t eps_status;
extern uint8_t pay_status;

//purpose of having indirect access to status is for using error checking
extern uint8_t* self_status;
extern uint8_t* parent_status;
extern uint8_t* child_status;

extern uint8_t ssm_id; //will be changed by each SSM
//obc {0x00} eps {10} pay {01}
extern uint8_t receiving_id;

extern mob_t status_rx_mob;
extern mob_t status_tx_mob;
//extern mob_id_tag_t id_tag;

//functions
void init_heartbeat();
void assign_heartbeat_status();
void assign_status_message_objects();
void heartbeat();

void rx_callback(uint8_t*, uint8_t);
void tx_callback(uint8_t*, uint8_t*);

#endif
