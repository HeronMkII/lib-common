#include "can_lib.h"

void can_send_message(uint8_t data[], uint8_t size, uint16_t id);
void init_rx_mob(* mob, uint8_t recieved_data[], uint8_t size, uint16_t id);
void init_rx_interrupts(st_cmd_t mob);

st_cmd_t rx_mob;


/* MASKS to read sender, reader and message type */
#define TX_MASK   0x0500
#define RX_MASK   0x01A0
#define MSG_MASK  0x003F

/* Example message: */
// uint16_t message_id = (OBC_TX | COM_RX | HK_REQ);

/* SENDER */
#define OBC_TX        0x0000
#define COM_TX        0x0200
#define EPS_TX        0x0400
#define PAY_TX        0x0500


/* RECEIVER */
#define OBC_RX        0x0000
#define COM_RX        0x0040
#define EPS_RX        0x0080
#define PAY_RX        0x00A0
#define ALL_RX        0x01A0

/* MESSAGE TYPE */
#define STATUS        0x0000
#define REQ_STATUS    0x0001
#define ACK_STATUS    0x0002

#define TC            0x0006
#define TM            0x0008

#define COMMAND_1     0x000A
#define COMMAND_2     0x000B
#define COMMAND_3     0x000C
#define COMMAND_4     0x000D
#define COMMAND_5     0x000E
#define COMMAND_6     0x000F

#define HK_DATA       0x0020

#define HK_SENSOR     0x0024

#define HK_REQ        0x0026

#define HK_REQ_SENSOR 0x0028

#define SCI_REQ       0x02A
#define SCI_DATA      0x02C
