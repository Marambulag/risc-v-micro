#include "hal_lcd.h"

// ==== DEFINES PARA PUERTOS Y PINES ====

// Puertos de datos (D4–D7 en PD0–PD3)
#define LCD_DATA_PORT     PORTD
#define LCD_DATA_GPIO     PTD
#define LCD_DATA_MCAL     MCAL_PORTD
#define LCD_D4_PIN        5
#define LCD_D5_PIN        4
#define LCD_D6_PIN        7
#define LCD_D7_PIN        6

// Puertos de control (RS, RW, EN en PA3–PA5)
#define LCD_CTRL_PORT     PORTA
#define LCD_CTRL_GPIO     PTA
#define LCD_CTRL_MCAL     MCAL_PORTA
#define LCD_RS_PIN        2
#define LCD_RW_PIN        4
#define LCD_EN_PIN        5

// ==== ESTRUCTURAS DE INFORMACIÓN DE PUERTO ====

static const MCAL_GPIO_Port_t DATA_PORT_INFO = {LCD_DATA_PORT, LCD_DATA_GPIO, LCD_DATA_MCAL};
static const MCAL_GPIO_Port_t CTRL_PORT_INFO = {LCD_CTRL_PORT, LCD_CTRL_GPIO, LCD_CTRL_MCAL};

// ==== CONFIGURACIÓN DE PINES ====

static const GPIO_Config_t data_cfg[4] = {
    {&DATA_PORT_INFO, LCD_D4_PIN, GPIO_DIR_OUTPUT, GPIO_PULL_NONE},
    {&DATA_PORT_INFO, LCD_D5_PIN, GPIO_DIR_OUTPUT, GPIO_PULL_NONE},
    {&DATA_PORT_INFO, LCD_D6_PIN, GPIO_DIR_OUTPUT, GPIO_PULL_NONE},
    {&DATA_PORT_INFO, LCD_D7_PIN, GPIO_DIR_OUTPUT, GPIO_PULL_NONE}
};

static const GPIO_Config_t rs_cfg = {&CTRL_PORT_INFO, LCD_RS_PIN, GPIO_DIR_OUTPUT, GPIO_PULL_NONE};
static const GPIO_Config_t rw_cfg = {&CTRL_PORT_INFO, LCD_RW_PIN, GPIO_DIR_OUTPUT, GPIO_PULL_NONE};
static const GPIO_Config_t en_cfg = {&CTRL_PORT_INFO, LCD_EN_PIN, GPIO_DIR_OUTPUT, GPIO_PULL_NONE};

// ==== MAIN ====

int main(void) {
    LCD_init(data_cfg, &rs_cfg, &rw_cfg, &en_cfg);
    while (1) {
        LCD_command(0x01);       // Clear display
        delayMs(500);
        LCD_command(0x80);       // Set cursor to beginning
        LCD_print("Hola Mundo");
        delayMs(2000);
    }
    return 0;
}
