#include "mcal_gpio.h"

void MCAL_GPIO_EnableClock(const MCAL_GPIO_Port_t *port_info) {
    SIM->SCGC5 |= port_info->clk_mask;
}

void MCAL_GPIO_ConfigOutput(const MCAL_GPIO_Port_t *port_info, uint32_t pin) {
    // ALT1 for GPIO
    port_info->port->PCR[pin] = PORT_PCR_MUX(1);
    // Set as output
    port_info->gpio->PDDR |= (1UL << pin);
}

void MCAL_GPIO_ConfigInput(const MCAL_GPIO_Port_t *port_info, uint32_t pin, GPIO_Pull_t pull)
{
    uint32_t pcr = PORT_PCR_MUX(1); // ALT1 = GPIO
    if (pull != GPIO_PULL_NONE) {
        pcr |= PORT_PCR_PE_MASK;
        if (pull == GPIO_PULL_UP) pcr |= PORT_PCR_PS_MASK;
        // pull-down => PS bit = 0
    }
    port_info->port->PCR[pin] = pcr;
    // Set as input
    port_info->gpio->PDDR &= ~(1UL << pin);
}

void MCAL_GPIO_WritePin(const MCAL_GPIO_Port_t *port_info, uint32_t pin, uint8_t value)
{
    if (value) port_info->gpio->PSOR = (1UL << pin);
    else       port_info->gpio->PCOR = (1UL << pin);
}

uint8_t MCAL_GPIO_ReadPin(const MCAL_GPIO_Port_t *port_info, uint32_t pin) {
	// TODO
	if (port_info->gpio->PDIR & (1UL << pin)) {
	    return 1;
	} else {
	    return 0;
	}
}
