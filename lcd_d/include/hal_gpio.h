#ifndef HAL_GPIO_H
#define HAL_GPIO_H

#include "mcal_gpio.h"

// GPIO direction
typedef enum {
    GPIO_DIR_INPUT,
    GPIO_DIR_OUTPUT
} GPIO_Direction_t;

// HAL configuration structure for a pin
typedef struct {
    const MCAL_GPIO_Port_t *port_info;
    uint32_t pin;
    GPIO_Direction_t direction;
    GPIO_Pull_t pull; // pull-up/down configuration
} GPIO_Config_t;

// Initialize pin as output
void HAL_GPIO_InitOutput(const GPIO_Config_t *cfg);

// Initialize pin as input with pull
void HAL_GPIO_InitInput(const GPIO_Config_t *cfg);

// Write pin state
void HAL_GPIO_Write(const GPIO_Config_t *cfg, uint8_t state);

// Read pin state
uint8_t HAL_GPIO_Read(const GPIO_Config_t *cfg);

#endif // HAL_GPIO_H

