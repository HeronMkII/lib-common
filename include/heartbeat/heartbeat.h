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
#define HB_RESTART_DATA_REQ 0x03
#define HB_RESTART_DATA_RESP 0x04

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

// TODO - update pin definitions for PAY outputs from flight model PCB

// PAY resets OBC
#define HB_PAY_RST_OBC_PIN  PB3
#define HB_PAY_RST_OBC_PORT PORTB
#define HB_PAY_RST_OBC_DDR  DDRB

// PAY resets EPS
#define HB_PAY_RST_EPS_PIN  PB3
#define HB_PAY_RST_EPS_PORT PORTB
#define HB_PAY_RST_EPS_DDR  DDRB

// Default 1 hour
#define HB_RESET_UPTIME_THRESH (1UL * 60UL * 60UL)


extern uint8_t hb_self_id;

extern mob_t obc_hb_mob;
extern mob_t eps_hb_mob;
extern mob_t pay_hb_mob;

extern volatile bool hb_send_obc_ping;
extern volatile bool hb_send_eps_ping;
extern volatile bool hb_send_pay_ping;

extern volatile bool hb_received_obc_resp;
extern volatile bool hb_received_eps_resp;
extern volatile bool hb_received_pay_resp;

extern volatile bool hb_send_obc_resp;
extern volatile bool hb_send_eps_resp;
extern volatile bool hb_send_pay_resp;

extern volatile bool hb_send_obc_rdata_req;
extern volatile bool hb_send_eps_rdata_req;
extern volatile bool hb_send_pay_rdata_req;

extern volatile bool hb_received_obc_rdata_resp;
extern volatile bool hb_received_eps_rdata_resp;
extern volatile bool hb_received_pay_rdata_resp;

extern volatile bool hb_send_obc_rdata_resp;
extern volatile bool hb_send_eps_rdata_resp;
extern volatile bool hb_send_pay_rdata_resp;

extern volatile uint32_t hb_ping_prev_uptime_s;
extern volatile uint32_t hb_ping_period_s;


void init_hb(uint8_t self_id);
bool wait_for_hb_mob_not_paused(mob_t* mob);
bool wait_for_hb_resp(volatile bool* received_resp);
void send_hb_resp(mob_t* mob, volatile bool* send_resp);
void send_hb_ping(mob_t* mob, uint8_t other_id, volatile bool* send_ping, volatile bool* received_resp);
bool send_hb_reset(uint8_t other_id);
void run_hb(void);

#endif // HEARTBEAT_H
