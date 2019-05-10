#ifndef HEARTBEAT_H
#define HEARTBEAT_H

#include <stdbool.h>
#include <stdint.h>

#include <can/can.h>

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
#define HB_OBC 0x00
#define HB_PAY 0x01
#define HB_EPS 0x02

// OBC resets EPS
#define HB_OBC_RST_EPS_PIN  PC4
#define HB_OBC_RST_EPS_PORT PORTC
#define HB_OBC_RST_EPS_DDR  DDRC

// OBC resets PAY
#define HB_OBC_RST_PAY_PIN  PC5
#define HB_OBC_RST_PAY_PORT PORTC
#define HB_OBC_RST_PAY_DDR  DDRC

// EPS resets OBC
#define HB_EPS_RST_OBC_PIN  PC6
#define HB_EPS_RST_OBC_PORT PORTC
#define HB_EPS_RST_OBC_DDR  DDRC

// EPS resets PAY
#define HB_EPS_RST_PAY_PIN  PC5
#define HB_EPS_RST_PAY_PORT PORTC
#define HB_EPS_RST_PAY_DDR  DDRC

// TODO - update pin definitions from PAY outputs

// PAY resets OBC
#define HB_PAY_RST_OBC_PIN  PD7
#define HB_PAY_RST_OBC_PORT PORTD
#define HB_PAY_RST_OBC_DDR  DDRD

// PAY resets EPS
#define HB_PAY_RST_EPS_PIN  PD7
#define HB_PAY_RST_EPS_PORT PORTD
#define HB_PAY_RST_EPS_DDR  DDRD


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

// Declare global variables for hb_self_id and receiving_id
// obc {0x00} eps {0x02} pay {0x01}
extern uint8_t hb_self_id;

extern uint8_t receiving_id;

extern mob_t status_rx_mob;
extern mob_t status_tx_mob;

// Declare fresh_start as global var. to keep track of fresh start and restart
extern uint8_t fresh_start; // 1 when board has a fresh start, 0 otherwise

// Other heartbeat functions
void assign_heartbeat_status(void);
void assign_status_message_objects(void);

// Declare heartbeat functions (Users only use the first 2)
void init_heartbeat(uint8_t id);
void heartbeat();

bool send_heartbeat_reset(uint8_t id);

#endif // HEARTBEAT_H
