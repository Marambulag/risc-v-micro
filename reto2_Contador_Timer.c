#include <MKL25Z4.h>
#include <stdio.h>

#define RS 0x04  // PTA2
#define RW 0x10  // PTA4
#define EN 0x20  // PTA5

void delayMs(int n);
void LCD_init(void);
void LCD_command(unsigned char cmd);
void LCD_data(unsigned char data);
void LCD_ready(void);
void TPM0_counter_init(void);

int main(void) {
    char buf[16];
    uint8_t curr = 0, prev = 0;
    uint32_t counter = 0;

    LCD_init();
    TPM0_counter_init();

    LCD_command(0x80);
    for (char *s = "Pulsos TPM0"; *s; s++) LCD_data(*s);

    while (1) {
        curr = (PTC->PDIR & (1<<12)) ? 1 : 0;
        // Flanco 0→1 detectado
        if (curr && !prev) {
            counter++;
            // Muestra valor (columna 8)
            LCD_command(0x88);
            sprintf(buf, "%5lu", counter);
            for (int i = 0; buf[i]; i++) LCD_data(buf[i]);
        }
        prev = curr;
        delayMs(20);  // debounce básico
    }
}

void TPM0_counter_init(void) {
    // 1. Habilitar reloj para PORTC y TPM0
    SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
    SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK;

    // 2. Configurar PTC12 como TPM0_CLKIN0 (ALT4) con pull-down interno
    PORTC->PCR[12] = (4 << PORT_PCR_MUX_SHIFT) | PORT_PCR_PE_MASK | PORT_PCR_PS(0);

    // 3. Seleccionar CLKIN0 como fuente externa para TPM0
    SIM->SOPT4 |= SIM_SOPT4_TPM0CLKSEL(1);  // Usar PTC12 (CLKIN0)

    // 4. Configurar TPM0 para contar flancos externos
    TPM0->SC = 0;           // Detener TPM0
    TPM0->CNT = 0;          // Reiniciar contador
    TPM0->MOD = 0xFFFF;     // Máximo valor (16 bits)

    // 5. Asegurar que la frecuencia de la señal externa <= (TPM_CLK / 2)
    TPM0->SC = TPM_SC_CMOD(2) | TPM_SC_PS(0);  // CMOD=10 (contador externo), PS=0 (sin prescaler)
}

uint16_t TPM0_read_and_clear(void) {
    uint16_t v = TPM0->CNT;
    TPM0->CNT = 0;
    return v;
}

// ========== Funciones para la LCD ==========
void LCD_init(void) {
    SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK | SIM_SCGC5_PORTA_MASK;
    for (int i = 0; i < 8; i++) PORTD->PCR[i] = PORT_PCR_MUX(1);
    PTD->PDDR = 0xFF;
    PORTA->PCR[2] = PORTA->PCR[4] = PORTA->PCR[5] = PORT_PCR_MUX(1);
    PTA->PDDR |= RS | RW | EN;

    delayMs(20);
    LCD_command(0x30); delayMs(5);
    LCD_command(0x30); delayMs(1);
    LCD_command(0x30);
    LCD_command(0x38);  // 8-bit, 2 líneas
    LCD_command(0x06);  // Incremento cursor
    LCD_command(0x0C);  // Display ON, cursor OFF
    LCD_command(0x01);  // Clear
    delayMs(2);
}

void LCD_ready(void) {
    PTD->PDDR = 0;
    PTA->PCOR = RS; PTA->PSOR = RW;
    uint8_t busy;
    do {
        PTA->PSOR = EN; delayMs(0);
        busy = PTD->PDIR & 0x80;
        PTA->PCOR = EN; delayMs(0);
    } while (busy);
    PTD->PDDR = 0xFF;
    PTA->PCOR = RW;
}

void LCD_command(unsigned char cmd) {
    LCD_ready();
    PTA->PCOR = RS | RW;
    PTD->PDOR = cmd;
    PTA->PSOR = EN; delayMs(0);
    PTA->PCOR = EN;
}

void LCD_data(unsigned char d) {
    LCD_ready();
    PTA->PSOR = RS; PTA->PCOR = RW;
    PTD->PDOR = d;
    PTA->PSOR = EN; delayMs(0);
    PTA->PCOR = EN;
}

void delayMs(int n) {
    SysTick->LOAD = 48000 - 1;
    SysTick->VAL = 0;
    SysTick->CTRL = 0x05;
    for (int i = 0; i < n; i++) while (!(SysTick->CTRL & 0x10000));
    SysTick->CTRL = 0;
}