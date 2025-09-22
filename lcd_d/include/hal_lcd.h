#ifndef HAL_LCD_H
#define HAL_LCD_H

#include "hal_gpio.h"
#include <stdint.h>

// Initialize LCD: data pins D4-D7, RS, RW, EN
void LCD_init(
    const GPIO_Config_t data_cfg[4],
    const GPIO_Config_t *rs_cfg,
    const GPIO_Config_t *rw_cfg,
    const GPIO_Config_t *en_cfg
);

// Send command
void LCD_command(uint8_t cmd);
// Send data
void LCD_data(uint8_t data);
// Print string
void LCD_print(const char *str);
// Delay ms
void delayMs(int ms);

#endif // HAL_LCD_H
