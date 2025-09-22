#include "hal_gpio.h"

void HAL_GPIO_InitOutput(const GPIO_Config_t *cfg) {
    MCAL_GPIO_EnableClock(cfg->port_info);
    MCAL_GPIO_ConfigOutput(cfg->port_info, cfg->pin);
}

void HAL_GPIO_InitInput(const GPIO_Config_t *cfg) {
    MCAL_GPIO_EnableClock(cfg->port_info);
    MCAL_GPIO_ConfigInput(cfg->port_info, cfg->pin, cfg->pull);
}

void HAL_GPIO_Write(const GPIO_Config_t *cfg, uint8_t state) {
    MCAL_GPIO_WritePin(cfg->port_info, cfg->pin, state);
}

uint8_t HAL_GPIO_Read(const GPIO_Config_t *cfg) {
    return MCAL_GPIO_ReadPin(cfg->port_info, cfg->pin);
}
