#ifndef PEX_EPS_H
#define PEX_EPS_H

#define PEX_CS_PIN_EPS      PB5
#define PEX_CS_PORT_EPS     PORTB
#define PEX_CS_DDR_EPS      DDRB
#define PEX_RST_PIN_EPS     PB4
#define PEX_RST_PORT_EPS    PORTB
#define PEX_RST_DDR_EPS     DDRB

// Shunts on GPIOA
// For battery charging (solar panel faces)
#define PEX_EPS_SHUNT_PX_PIN    0   // +X face
#define PEX_EPS_SHUNT_NX_PIN    1   // -X face
#define PEX_EPS_SHUNT_PY_PIN    2   // +Y face
#define PEX_EPS_SHUNT_NY_PIN    3   // -Y face

#endif  // PEX_EPS_H
