#include "hal_lcd.h"
#include <MKL25Z4.h>

static GPIO_Config_t lcd_data[4];
static GPIO_Config_t lcd_rs;
static GPIO_Config_t lcd_rw;
static GPIO_Config_t lcd_en;

void delayMs(int ms) {
    volatile int i;
    while (ms-- > 0) for (i = 0; i < 7000; i++) __NOP();
}

static void LCD_sendNibble(uint8_t nib, uint8_t rs) {
    // Write 4 data pins
    for (int i = 0; i < 4; i++) {
        HAL_GPIO_Write(&lcd_data[i], (nib >> i) & 1);
    }
    HAL_GPIO_Write(&lcd_rs, rs);
    HAL_GPIO_Write(&lcd_rw, 0);
    HAL_GPIO_Write(&lcd_en, 1);
    delayMs(1);
    HAL_GPIO_Write(&lcd_en, 0);
    delayMs(1);
}

void LCD_init(
    const GPIO_Config_t data_cfg[4],
    const GPIO_Config_t *rs_cfg,
    const GPIO_Config_t *rw_cfg,
    const GPIO_Config_t *en_cfg
) {
    // Store configs
    for (int i = 0; i < 4; i++) lcd_data[i] = data_cfg[i];
    lcd_rs = *rs_cfg;
    lcd_rw = *rw_cfg;
    lcd_en = *en_cfg;

    // Initialize pins
    for (int i = 0; i < 4; i++) HAL_GPIO_InitOutput(&lcd_data[i]);
    HAL_GPIO_InitOutput(rs_cfg);
    HAL_GPIO_InitOutput(rw_cfg);
    HAL_GPIO_InitOutput(en_cfg);

    // Init sequence
    delayMs(40);
    LCD_sendNibble(0x03, 0); delayMs(5);
    LCD_sendNibble(0x03, 0); delayMs(1);
    LCD_sendNibble(0x03, 0); delayMs(1);
    LCD_sendNibble(0x02, 0);

    LCD_command(0x28);
    LCD_command(0x06);
    LCD_command(0x01);
    LCD_command(0x0C);
}

void LCD_command(uint8_t cmd) {
    LCD_sendNibble(cmd >> 4, 0);
    LCD_sendNibble(cmd & 0x0F, 0);
    if (cmd == 0x01 || cmd == 0x02) delayMs(2);
}

void LCD_data(uint8_t data) {
    LCD_sendNibble(data >> 4, 1);
    LCD_sendNibble(data & 0x0F, 1);
}

void LCD_print(const char *str) {
    while (*str) LCD_data((uint8_t)*str++);
}

