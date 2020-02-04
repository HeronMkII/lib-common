#ifndef HEARTBEAT_H
#define HEARTBEAT_H

#include <stdbool.h>
#include <stdint.h>

#include <can/can.h>
#include <can/ids.h>
#include <uptime/uptime.h>

// OpCodes
#define HB_PING_REQUEST 0x01
#define HB_PING_RESPONSE 0x02

// Heartbeat Byte structure
#define HB_SENDER 0x00
#define HB_RECEIVER 0x01
#define HB_OPCODE 0x02
#define HB_RESTART_REASON 0x03
#define HB_RESTART_COUNT 0x04

// Define SSM ids
#define HB_OBC 0x00
#define HB_PAY 0x01
#define HB_EPS 0x02

#define HB_NUM_DEVS 3

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

// PAY resets OBC
// NOTE: On older boards (e.g. 2018-06 PAY-SSM, 2019-02 OBC), PC0 is connected
// to the CAN transceiver LBK (loopback) pin, which will disconnect the CAN
// transceiver from the bus and cause all CAN messages to fail (no ack) as it is
// nominally held high in the heartbeat reset configuration
// If testing on one of those older boards, change the constants to
// PB3/PORTB/DDRB
#define HB_PAY_RST_OBC_PIN  PC0
#define HB_PAY_RST_OBC_PORT PORTC
#define HB_PAY_RST_OBC_DDR  DDRC

// PAY resets EPS
#define HB_PAY_RST_EPS_PIN  PD1
#define HB_PAY_RST_EPS_PORT PORTD
#define HB_PAY_RST_EPS_DDR  DDRD

// Default 3 hours
#define HB_REQ_PERIOD_S    (3UL * 60UL * 60UL)
// Number of seconds to wait for a response before sending a reset
#define HB_RESP_WAIT_TIME_S 60


// Heartbeat device informataion
typedef struct {
    char name[4];
    uint8_t id;
    mob_t mob;

    // When initiating request
    bool ping_in_progress;
    bool send_req_flag;
    bool rcvd_resp_flag;

    // When responding to request
    bool send_resp_flag;
    
    pin_info_t* reset;
    uint32_t ping_start_uptime_s;
    uint8_t restart_reason;
    uint32_t restart_count;
} hb_dev_t;

// Must all be volatile because they are modified inside CAN TX/RX interrupts
// Don't initialize mobs here
extern volatile hb_dev_t obc_hb_dev;
extern volatile hb_dev_t eps_hb_dev;
extern volatile hb_dev_t pay_hb_dev;

extern volatile hb_dev_t* all_hb_devs[];

extern volatile hb_dev_t* self_hb_dev;

extern volatile uint32_t hb_req_prev_uptime_s;
extern volatile uint32_t hb_req_period_s;
extern volatile uint32_t hb_resp_wait_time_s;


void init_hb(uint8_t self_id);
bool wait_for_hb_mob_not_paused(mob_t* mob);
bool send_hb_reset(hb_dev_t* device);
void run_hb(void);

#endif // HEARTBEAT_H
