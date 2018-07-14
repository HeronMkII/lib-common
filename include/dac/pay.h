#ifndef DAC_PAY_H
#define DAC_PAY_H

// PAY-SSM
#define DAC_CS_PIN_PAY          PD0
#define DAC_CS_PORT_PAY         PORTD
#define DAC_CS_DDR_PAY          DDRD
#define DAC_CLR_PIN_PAY         PD1
#define DAC_CLR_PORT_PAY        PORTD
#define DAC_CLR_DDR_PAY         DDRD

// PAY-Optical
#define DAC_CS_PIN_PAY_OPT      PB4
#define DAC_CS_PORT_PAY_OPT     PORTB
#define DAC_CS_DDR_PAY_OPT      DDRB
// The DAC_CLR pin on PAY-Optical is not connected
// Set it to some random pin
#define DAC_CLR_PIN_PAY_OPT     PB0
#define DAC_CLR_PORT_PAY_OPT    PORTB
#define DAC_CLR_DDR_PAY_OPT     DDRB

#endif  // DAC_PAY_H
