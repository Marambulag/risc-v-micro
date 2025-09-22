#include <MKL25Z4.h>
#include <stdio.h>
#include <stdlib.h>

#define RS 0x04 /* PTA2 mask */
#define RW 0x10 /* PTA4 mask */
#define EN 0x20 /* PTA5 mask */

// Prototipos de funciones
void ADC0_init(void);
void delayMs(int n);
void LCD_command(unsigned char command);
void LCD_command_noWait(unsigned char command);
void LCD_data(unsigned char data);
void LCD_init(void);
void keypad_init(void);
char keypad_getkey(void);
int decoder_teclado(char input);
int getDesiredTemp(void);
void LED_init(void);
void displayCurrentTemp(float temperature);
void checkAndDisplayTemp(float current, int desired);

int main(void) {
    int desiredTemp = 25; // Valor por defecto
    float currentTemp;

    // Inicialización de hardware
    SIM->SCGC6 |= 0x01000000; /* enable clock to TPM0 */
    SIM->SOPT2 |= 0x01000000; /* use 32.76khz as clock */
    TPM0->SC = 0; /* disable timer while configuring */
    TPM0->SC = 0x02; /* prescaler /4 */
    TPM0->MOD = 0x2000; /* max modulo value 8192*/
    TPM0->SC |= 0x80; /* clear TOF */
    TPM0->SC |= 0x08; /* enable timer free-running mode */

    ADC0_init();
    LCD_init();
    keypad_init();
    LED_init();

    // Paso 1: Pedir temperatura deseada
    desiredTemp = getDesiredTemp();

    while(1) {
        // Paso 2: Mostrar temperatura actual y comparación
        ADC0->SC1[0] = 0; // Usar canal 0 para LM35
        while(!(ADC0->SC1[0] & 0x80)) { } /* wait for COCO */
        int result = ADC0->R[0];
        currentTemp = ((result * 330)/65536); // Convertir a temperatura

        LCD_command(1); // Limpiar pantalla
        displayCurrentTemp(currentTemp);
        checkAndDisplayTemp(currentTemp, desiredTemp);

        // Esperar 2 segundos antes de volver a pedir temperatura deseada
        delayMs(2000);

        // Volver a pedir temperatura deseada
        desiredTemp = getDesiredTemp();
    }
}

// Función mejorada para obtener temperatura deseada
int getDesiredTemp(void) {
    char str[3];
    int temp = 0;
    int digitCount = 0;
    char key;

    LCD_command(1); // Limpiar pantalla
    LCD_command(0x80);
    LCD_data('E');
    LCD_data('n');
    LCD_data('t');
    LCD_data('e');
    LCD_data('r');
    LCD_data(' ');
    LCD_data('T');
    LCD_data('e');
    LCD_data('m');
    LCD_data('p');
    LCD_data(':');

    LCD_command(0xC0); // Segunda línea

    while(1) {
        key = keypad_getkey();
        if(key > 0) {
            int num = decoder_teclado(key);

            // Solo procesar dígitos 0-9
            if(num >= 0 && num <= 9 && digitCount < 2) {
                temp = temp * 10 + num;
                digitCount++;

                // Mostrar dígito ingresado
                LCD_data('0' + num);
            }
            // Si se presiona * y tenemos al menos 1 dígito
            else if(key == 13 && digitCount > 0) {
                return temp; // Retornar temperatura ingresada
            }
            // Si se presiona #, cancelar
            else if(key == 15) {
                return -1; // Valor inválido
            }
        }
        delayMs(100);
    }
}

// Mostrar temperatura actual
void displayCurrentTemp(float temperature) {
    char str[16];
    int temp_int = (int)temperature;
    int temp_frac = (int)((temperature - temp_int)*10);

    LCD_command(0x80); // Primera línea
    LCD_data('C');
    LCD_data('u');
    LCD_data('r');
    LCD_data('r');
    LCD_data(':');
    LCD_data(' ');

    sprintf(str, "%d.%dC", temp_int, temp_frac);
    for(int i=0; str[i]!='\0'; i++) {
        LCD_data(str[i]);
    }
}

// Verificar y mostrar comparación
void checkAndDisplayTemp(float current, int desired) {
    LCD_command(0xC0); // Segunda línea

    if(desired < 0) {
        LCD_data('I');
        LCD_data('n');
        LCD_data('v');
        LCD_data('a');
        LCD_data('l');
        LCD_data('i');
        LCD_data('d');
        return;
    }

    if(current < desired - 1) {
        LCD_data('T');
        LCD_data('o');
        LCD_data('o');
        LCD_data(' ');
        LCD_data('C');
        LCD_data('o');
        LCD_data('l');
        LCD_data('d');
        PTD->PCOR = 0x02; // LED azul
    }
    else if(current > desired + 1) {
        LCD_data('T');
        LCD_data('o');
        LCD_data('o');
        LCD_data(' ');
        LCD_data('H');
        LCD_data('o');
        LCD_data('t');
        PTB->PCOR = 0x40000; // LED rojo
    }
    else {
        LCD_data('O');
        LCD_data('K');
        LCD_data(' ');
        LCD_data('T');
        LCD_data('e');
        LCD_data('m');
        LCD_data('p');
        PTB->PCOR = 0x80000; // LED verde
    }
}

// Resto de las funciones se mantienen igual que en el código anterior
// (ADC0_init, LCD_init, LCD_ready, LCD_command, LCD_command_noWait,
// LCD_data, keypad_init, keypad_getkey, decoder_teclado, LED_init, delayMs)

void ADC0_init(void) {
    SIM->SCGC5 |= 0x2000; /* clock to PORTE */
    PORTE->PCR[20] = 0; /* PTE20 analog input */
    SIM->SCGC6 |= 0x8000000; /* clock to ADC0 */
    ADC0->SC2 &= ~0x40; /* software trigger */
    ADC0->SC3 |= 0x07; /* 32 samples average */
    ADC0->CFG1 = 0x40 | 0x10 | 0x0C | 0x00;
}

void LCD_init(void) {
    SIM->SCGC5 |= 0x1000; /* enable clock to Port D */
    PORTD->PCR[0] = 0x100; PORTD->PCR[1] = 0x100;
    PORTD->PCR[2] = 0x100; PORTD->PCR[3] = 0x100;
    PORTD->PCR[4] = 0x100; PORTD->PCR[5] = 0x100;
    PORTD->PCR[6] = 0x100; PORTD->PCR[7] = 0x100;
    PTD->PDDR = 0xFF;
    SIM->SCGC5 |= 0x0200; /* enable clock to Port A */
    PORTA->PCR[2] = 0x100; PORTA->PCR[4] = 0x100;
    PORTA->PCR[5] = 0x100;
    PTA->PDDR |= 0x34;

    delayMs(20);
    LCD_command_noWait(0x30); delayMs(5);
    LCD_command_noWait(0x30); delayMs(1);
    LCD_command_noWait(0x30);
    LCD_command(0x38); LCD_command(0x06);
    LCD_command(0x01); LCD_command(0x0F);
}

void LCD_ready(void) {
    char status;
    PTD->PDDR = 0;
    PTA->PCOR = RS; PTA->PSOR = RW;
    do {
        PTA->PSOR = EN; delayMs(0);
        status = PTD->PDIR;
        PTA->PCOR = EN; delayMs(0);
    } while (status & 0x80);
    PTA->PCOR = RW; PTD->PDDR = 0xFF;
}

void LCD_command(unsigned char command) {
    LCD_ready();
    PTA->PCOR = RS | RW;
    PTD->PDOR = command;
    PTA->PSOR = EN; delayMs(0);
    PTA->PCOR = EN;
}

void LCD_command_noWait(unsigned char command) {
    PTA->PCOR = RS | RW;
    PTD->PDOR = command;
    PTA->PSOR = EN; delayMs(0);
    PTA->PCOR = EN;
}

void LCD_data(unsigned char data) {
    LCD_ready();
    PTA->PSOR = RS; PTA->PCOR = RW;
    PTD->PDOR = data;
    PTA->PSOR = EN; delayMs(0);
    PTA->PCOR = EN;
}

void keypad_init(void) {
    SIM->SCGC5 |= 0x0800;
    PORTC->PCR[0] = 0x103; PORTC->PCR[1] = 0x103;
    PORTC->PCR[2] = 0x103; PORTC->PCR[3] = 0x103;
    PORTC->PCR[4] = 0x103; PORTC->PCR[5] = 0x103;
    PORTC->PCR[6] = 0x103; PORTC->PCR[7] = 0x103;
    PTC->PDDR = 0x0F;
}

char keypad_getkey(void) {
    int row, col;
    const char row_select[] = {0x01, 0x02, 0x04, 0x08};

    PTC->PDDR |= 0x0F; PTC->PCOR = 0x0F;
    delayMs(2); col = PTC->PDIR & 0xF0;
    PTC->PDDR = 0; if(col == 0xF0) return 0;

    for(row=0; row<4; row++) {
        PTC->PDDR = 0; PTC->PDDR |= row_select[row];
        PTC->PCOR = row_select[row]; delayMs(2);
        col = PTC->PDIR & 0xF0; if(col != 0xF0) break;
    }

    PTC->PDDR = 0; if(row == 4) return 0;

    if(col == 0xE0) return row*4 + 1;
    if(col == 0xD0) return row*4 + 2;
    if(col == 0xB0) return row*4 + 3;
    if(col == 0x70) return row*4 + 4;
    return 0;
}

int decoder_teclado(char input) {
    if(input == 0) return -3;
    else if(input == 1) { LCD_data('1'); return 1; }
    else if(input == 2) { LCD_data('2'); return 2; }
    else if(input == 3) { LCD_data('3'); return 3; }
    else if(input == 4) { LCD_data('A'); return 10; }
    else if(input == 5) { LCD_data('4'); return 4; }
    else if(input == 6) { LCD_data('5'); return 5; }
    else if(input == 7) { LCD_data('6'); return 6; }
    else if(input == 8) { LCD_data('B'); return 11; }
    else if(input == 9) { LCD_data('7'); return 7; }
    else if(input == 10) { LCD_data('8'); return 8; }
    else if(input == 11) { LCD_data('9'); return 9; }
    else if(input == 12) { LCD_data('C'); return 12; }
    else if(input == 13) { LCD_data('*'); return -1; }
    else if(input == 14) { LCD_data('0'); return 0; }
    else if(input == 15) { LCD_data('#'); return -2; }
    else if(input == 16) { LCD_data('D'); return 13; }
    else return -3;
}

void LED_init(void) {
    SIM->SCGC5 |= 0x400; SIM->SCGC5 |= 0x1000;
    PORTB->PCR[18] = 0x100; PTB->PDDR |= 0x40000; PTB->PSOR |= 0x40000;
    PORTB->PCR[19] = 0x100; PTB->PDDR |= 0x80000; PTB->PSOR |= 0x80000;
    PORTD->PCR[1] = 0x100; PTD->PDDR |= 0x02; PTD->PSOR |= 0x02;
}

void delayMs(int n) {
    while((TPM0->SC & 0x80) == 0) { }
    int i;
    SysTick->LOAD = 41940 - 1;
    SysTick->CTRL = 0x5;
    for(i=0; i<n/2; i++) {
        while((SysTick->CTRL & 0x10000) == 0) { }
    }
    SysTick->CTRL = 0;
}
