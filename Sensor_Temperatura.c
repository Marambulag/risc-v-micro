#include <MKL25Z4.H>
#include <stdio.h>
#include <stdlib.h>

#define RS 0x04 /* PTA2 mask */
#define RW 0x10 /* PTA4 mask */
#define EN 0x20 /* PTA5 mask */

void ADC0_init(void);
void delayMs(int n);
//void UART0_init(void);
//void UART0Tx(char c);
//void UART0_puts(char* s);

void LCD_command(unsigned char command);

void LCD_command_noWait(unsigned char command);

void LCD_data(unsigned char data);

void LCD_init(void);


int main (void) {

	/* Init code */
		SIM->SCGC6 |= 0x01000000; /* enable clock to TPM0 */
		SIM->SOPT2 |= 0x01000000; /* use 32.76khz as clock */
		TPM0->SC = 0; /* disable timer while configuring */
		TPM0->SC = 0x02; /* prescaler /4 */
		TPM0->MOD = 0x2000; /* max modulo value 8192*/
		TPM0->SC |= 0x80; /* clear TOF */
		TPM0->SC |= 0x08; /* enable timer free-running mode */

int result;
float temperature;
char str[16];
ADC0_init(); /* Configure ADC0 */
//UART0_init(); /* initialize UART0 for output */
LCD_init();

while (1) {
	LCD_command(1);    /* clear display */
ADC0->SC1[0] = 26; /* Si vas a usar el interno, ponlo en 26. Si vas a usar el LM35 usa el 0. (Capacitor) */
while(!(ADC0->SC1[0] & 0x80)) { } /* wait for COCO */
result = ADC0->R[0];
/* read conversion result and clear COCO flag */
temperature = (( result * 330)/65536);
/* convert voltage to temperature */

int temp_int = (int)temperature;

//Imprimir el timer
		sprintf(str, "%d", temp_int);
		for (int i = 0; str[i] != '\0'; i++) {
			LCD_data(str[i]);
		}

//sprintf(buffer, “\r\nTemp = %6.2fF”, temperature);
/* convert to string */
//UART0_puts(buffer);
/* send the string through UART0 for display */

delayMs(500);
} }

void ADC0_init(void)
{
SIM->SCGC5 |= 0x2000; /* clock to PORTE */
PORTE->PCR[20] = 0; /* PTE20 analog input */
SIM->SCGC6 |= 0x8000000; /* clock to ADC0 */
ADC0->SC2 &= ~0x40; /* software trigger */
ADC0->SC3 |= 0x07; /* 32 samples average */
/* clock div by 4, long sample time, single ended 16 bit,
bus clock */
ADC0->CFG1 = 0x40 | 0x10 | 0x0C | 0x00;
}


void LCD_init(void){
	SIM->SCGC5 |= 0x1000; /* enable clock to Port D */
	PORTD->PCR[0] = 0x100;/* make PTD0 pin as GPIO */
	PORTD->PCR[1] = 0x100;/* make PTD1 pin as GPIO */
	PORTD->PCR[2] = 0x100;
	PORTD->PCR[3] = 0x100;
	PORTD->PCR[4] = 0x100;
	PORTD->PCR[5] = 0x100;
	PORTD->PCR[6] = 0x100;
	PORTD->PCR[7] = 0x100;/* make PTD7 pin as GPIO */
	PTD->PDDR = 0xFF;  /* make PTD7-0 as output pins */
	SIM->SCGC5 |= 0x0200; /* enable clock to Port A */
	PORTA->PCR[2] = 0x100;/* make PTA2 pin as GPIO */
	PORTA->PCR[4] = 0x100;/* make PTA4 pin as GPIO */
	PORTA->PCR[5] = 0x100;/* make PTA5 pin as GPIO */
	PTA->PDDR |= 0x34; /make PTA5, 4, 2 as output pins/

	delayMs(20); /* initialization sequence */
	/* LCD does not respond to status poll */
	LCD_command_noWait(0x30);
	delayMs(5);
	LCD_command_noWait(0x30);
	delayMs(1);
	LCD_command_noWait(0x30);
	/* set 8-bit data, 2-line, 5x7 font */
	LCD_command(0x38);
	/* move cursor right */
	LCD_command(0x06);
	/* clear screen, move cursor to home */
	LCD_command(0x01);
	/* turn on display, cursor blinking */
	LCD_command(0x0F);
}

/* This function waits until LCD controller is ready to accept a new command/data before returns. */
void LCD_ready(void)
{
	char status;
	PTD->PDDR = 0; /* PortD input */
	PTA->PCOR = RS; /* RS = 0 for status */
	PTA->PSOR = RW; /* R/W = 1, LCD output */
	do { /* stay in the loop until it is not busy */
		PTA->PSOR = EN; /* raise E */
		delayMs(0);
		status = PTD->PDIR; /* read status register */
		PTA->PCOR = EN;
		delayMs(0); /* clear E */
	} while (status & 0x80); /* check busy bit */


	PTA->PCOR = RW; /* R/W = 0, LCD input */
	PTD->PDDR = 0xFF; /* PortD output */
}

void LCD_command(unsigned char command)
{
	LCD_ready(); /* wait until LCD is ready */
	PTA->PCOR = RS | RW; /* RS = 0, R/W = 0 */
	PTD->PDOR = command;
	PTA->PSOR = EN; /* pulse E */
	delayMs(0);
	PTA->PCOR = EN;
}

void LCD_command_noWait(unsigned char command){
	PTA->PCOR = RS | RW; /* RS = 0, R/W = 0 */
	PTD->PDOR = command;
	PTA->PSOR = EN; /* pulse E */
	delayMs(0);
	PTA->PCOR = EN; }

void LCD_data(unsigned char data)
{
	LCD_ready(); /* wait until LCD is ready */
	PTA->PSOR = RS; /* RS = 1, R/W = 0 */
	PTA->PCOR = RW;
	PTD->PDOR = data;
	PTA->PSOR = EN; /* pulse E */
	delayMs(0);
	PTA->PCOR = EN;
}

/* Delay function */
void delayMs(int n) {
	while((TPM0->SC & 0x80) == 0) { }
	int i;
	SysTick->LOAD = 41940 - 1;
	SysTick->CTRL = 0x5; /* Enable the timer and
	choose sysclk as the clock
	source */
	for(i = 0; i < n/2; i++) {
	while((SysTick->CTRL & 0x10000) == 0)
	/* wait until the COUNT flag is set */
	{ }
	}
	SysTick->CTRL = 0;
	/* Stop the timer (Enable = 0) */
	}
