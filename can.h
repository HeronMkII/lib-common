#include "can_lib.h"
// #include <"avr/interrupt.h">

void can_send_message(uint8_t data[], uint8_t size, uint8_t id);
void init_rx_mob(uint8_t size, uint8_t id);
void init_rx_interrupts();


// Message IDs 

#define OBC_STATUS 				20
#define COM_STATUS 				21
#define EPS_STATUS 				22
#define PAY_STATUS 				23

#define OBC_REQ_STATUS_ALL		30
#define COM_REQ_STATUS_ALL		31
#define EPS_REQ_STATUS_ALL		32
#define PAY_REQ_STATUS_ALL		33

#define COM_TC					40
#define OBC_TM					41

#define OBC_EPS_COMMANDS		60

#define OBC_PAY_COMMANDS		80

#define COM_HK					100
#define EPS_HK  				101
#define PAY_HK 					102

#define COM_HK_SENSOR			110
#define EPS_HK_SENSOR			111
#define PAY_HK_SENSOR			112

#define OBC_REQ_HK_ALL			120
#define OBC_REQ_HK_COM			121
#define OBC_REQ_HK_EPS			122
#define OBC_REQ_HK_PAY			123

#define OBC_REQ_HK_SENSOR_COM	130
#define OBC_REQ_HK_SENSOR_EPS	131
#define OBC_REQ_HK_SENSOR_PAY	132

#define OBC_REQ_SCIENCE			200
#define PAY_SCIENCE				210

