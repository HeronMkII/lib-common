#ifndef HEARTBEAT_H
#define HEARTBEAT_H

// CAN ids of each SSMs.
// Currently DOES NOT follow the convention of can_ids in lib-common master
#define OBC_STATUS_RX_MOB_ID 0x001c
#define OBC_STATUS_TX_MOB_ID 0x000b
#define EPS_STATUS_RX_MOB_ID 0x001b
#define EPS_STATUS_TX_MOB_ID 0x000a
#define PAY_STATUS_RX_MOB_ID 0x001a
#define PAY_STATUS_TX_MOB_ID 0x000c

// Use DEADBEEF to keep track of fresh start or restart
#define DEADBEEF 0xdeadbeef //4 bytes

// Define SSM ids
#define OBC 0x00
#define EPS 0x02
#define PAY 0x01

// Store SSM status as global variables
extern uint8_t obc_status;
extern uint8_t eps_status;
extern uint8_t pay_status;

// Declare global pointers to generalized SSM status
/*Purpose of having indirect access to status is for using error checking to
reinforce the parent-child relationship among each SSM*/
extern uint8_t* self_status;
extern uint8_t* parent_status;
extern uint8_t* child_status;

// Declare global variables for ssm_id and receiving_id
// obc {0x00} eps {0x02} pay {0x01}
extern uint8_t ssm_id;
extern uint8_t receiving_id;

extern mob_t status_rx_mob;
extern mob_t status_tx_mob;

// Declare fresh_start as global var. to keep track of fresh start and restart
extern uint8_t fresh_start; // 1 when board has a fresh start, 0 otherwise

// Declare heartbeat functions (Users only use the first 2)
void init_heartbeat();
void heartbeat();

#endif // HEARTBEAT_H
