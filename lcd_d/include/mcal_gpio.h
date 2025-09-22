#ifndef MCAL_GPIO_H
#define MCAL_GPIO_H

#include <MKL25Z4.h>
#include <stdint.h>

// Clock mask identifiers for SIM->SCGC5
typedef enum {
    MCAL_PORTA = SIM_SCGC5_PORTA_MASK,
    MCAL_PORTB = SIM_SCGC5_PORTB_MASK,
    MCAL_PORTC = SIM_SCGC5_PORTC_MASK,
    MCAL_PORTD = SIM_SCGC5_PORTD_MASK,
    MCAL_PORTE = SIM_SCGC5_PORTE_MASK
} MCAL_PortClockMask_t;

// Generic port information structure
typedef struct {
    PORT_Type *port;    // PCR registers base
    GPIO_Type *gpio;    // PDDR/PDIR/PSOR/PCOR registers base
    MCAL_PortClockMask_t clk_mask;
} MCAL_GPIO_Port_t;

// Pull resistor configuration
typedef enum {
    GPIO_PULL_NONE,
    GPIO_PULL_UP,
    GPIO_PULL_DOWN
} GPIO_Pull_t;

// Enable clock for given port
void MCAL_GPIO_EnableClock(const MCAL_GPIO_Port_t *port_info);

// Configure pin for digital output
void MCAL_GPIO_ConfigOutput(const MCAL_GPIO_Port_t *port_info, uint32_t pin);

// Configure pin for digital input with optional pull
void MCAL_GPIO_ConfigInput(
    const MCAL_GPIO_Port_t *port_info,
    uint32_t pin,
    GPIO_Pull_t pull);

// Write pin: 1 sets, 0 clears
void MCAL_GPIO_WritePin(
    const MCAL_GPIO_Port_t *port_info,
    uint32_t pin,
    uint8_t value);

// Read pin: returns 1 if high, 0 if low
uint8_t MCAL_GPIO_ReadPin(
    const MCAL_GPIO_Port_t *port_info,
    uint32_t pin);

#endif // MCAL_GPIO_H

