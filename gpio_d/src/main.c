#include "hal_gpio.h"
#include <MKL25Z4.h>

// Example port definitions
static const MCAL_GPIO_Port_t PORTB_INFO = {PORTB, PTB, MCAL_PORTB};

// LED and button pin definitions
#define RED_LED_PIN    19
#define BUTTON_PIN     2

// ON/OFF Macros
#define ON 1
#define OFF 0

typedef enum{
	GPIO_DIR_INPUT,
	GPIO_DIR_OUTPUT
} GPIO_Direction_t;

typedef struct{
	const MCAL_GPIO_Port_t *port_info;
	uint32_t pin;
	GPIO_Direction_t direction;
}GPIO_Config_t;

static const GPIO_Config_t config_table[] = {
    {&PORTB_INFO, RED_LED_PIN, GPIO_DIR_OUTPUT},
    {&PORTB_INFO, BUTTON_PIN, GPIO_DIR_INPUT}
};

void HAL_GPIO_Driver_Init(const GPIO_Config_t *config_table, uint32_t num_entries){
	for(uint32_t i=0; i<num_entries; i++){
		if(config_table[i].direction == GPIO_DIR_OUTPUT){
			HAL_GPIO_InitOutput(config_table[i].port_info, config_table[i].pin);
		}
		else{
			HAL_GPIO_InitInput(config_table[i].port_info, config_table[i].pin);
		}
	}
}

int main(void) {
    HAL_GPIO_Driver_Init(config_table, sizeof(config_table)/sizeof(GPIO_Config_t));

    while (1) {
        uint8_t button_state = HAL_GPIO_Read(&PORTB_INFO, BUTTON_PIN);

        if (button_state == 0) {
            HAL_GPIO_Write(&PORTB_INFO, RED_LED_PIN, OFF);
        } else {
            HAL_GPIO_Write(&PORTB_INFO, RED_LED_PIN, ON);
        }
    }
}
