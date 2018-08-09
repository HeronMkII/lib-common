#ifndef SPI_H
#define SPI_H

#include <avr/io.h>
#include <avr/cpufunc.h> // for _NOP()
#include <string.h>

// SPCR bits
#define SPIE 7 // enable spi interrupts
#define SPE  6 // enable spi
#define DORD 5 // data order; 1 - LSB first, 0 - MSB fist
#define MSTR 4 // enable master SPI mode
#define CPOL 3 // clock polarity
#define CPHA 2 // clock phase
#define SPR1 1 // SPI clock rate select 1
#define SPR0 0 // SPI clock rate select 0

// SPSR bits
#define SPIF 7 // SPI interrupt flag; set to 1 when SPI transfer completes
#define WCOL 6 // write collision flag
#define SPI2X 0 // double speed bit

typedef volatile uint8_t* pint_t;
typedef volatile uint8_t* port_t;

typedef struct {
    port_t port;
    port_t ddr;
    uint8_t pin;
} pin_info_t;

void init_spi(void);
uint8_t send_spi(uint8_t);

void init_cs(uint8_t, port_t);
void set_cs_low(uint8_t, port_t);
void set_cs_high(uint8_t, port_t);

#endif
