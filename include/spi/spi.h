#ifndef SPI_H
#define SPI_H

#include <avr/io.h>
#include <avr/cpufunc.h> // for _NOP()
#include <string.h>

// SPCR bits
#define SPIE    7 // enable spi interrupts
#define SPE     6 // enable spi
#define DORD    5 // data order; 1 - LSB first, 0 - MSB fist
#define MSTR    4 // enable master SPI mode
#define CPOL    3 // clock polarity
#define CPHA    2 // clock phase
#define SPR1    1 // SPI clock rate select 1
#define SPR0    0 // SPI clock rate select 0

// SPSR bits
#define SPIF    7 // SPI interrupt flag; set to 1 when SPI transfer completes
#define WCOL    6 // write collision flag
#define SPI2X   0 // double speed bit

// Memory-mapped registers for PORTx and DDRx
// Volatile because the register hardware can change without the program knowing
typedef volatile uint8_t* port_t;
typedef volatile uint8_t* ddr_t;

// Groups the variables needed to manipulate (read/write) a pin
typedef struct {
    port_t port;
    ddr_t ddr;
    uint8_t pin;
} pin_info_t;

// Possible settings for SPI clock frequency (p. 221)
// F_osc (oscillator/clock frequency) is 8 MHz
typedef enum {
    // F_osc / 4
    SPI_FOSC_4,
    // F_osc / 16
    SPI_FOSC_16,
    // F_osc / 64
    SPI_FOSC_64,
    // F_osc / 128
    SPI_FOSC_128,
    // F_osc / 2
    SPI_FOSC_2,
    // F_osc / 8
    SPI_FOSC_8,
    // F_osc / 32
    SPI_FOSC_32
    // Don't include FOSC_64 again
} spi_clk_freq_t;

void init_cs(uint8_t, ddr_t);
void set_cs_low(uint8_t, port_t);
void set_cs_high(uint8_t, port_t);

void init_spi(void);
uint8_t send_spi(uint8_t);
uint16_t send_spi_2bytes(uint16_t data);
uint32_t send_spi_3bytes(uint32_t data);

// SPI data mode (polarity/phase)
void set_spi_cpol_cpha(uint8_t cpol, uint8_t cpha);
void reset_spi_cpol_cpha(void);
void set_spi_mode(uint8_t mode);
void reset_spi_mode(void);

// SPI clock frequency
void set_spi_spi2x_spr1_spr0(uint8_t spi2x, uint8_t spr1, uint8_t spr0);
void set_spi_clk_freq(spi_clk_freq_t freq);
void reset_spi_clk_freq(void);

#endif
