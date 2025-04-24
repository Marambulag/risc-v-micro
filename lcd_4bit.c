#include <MKL25Z4.h>  // Cabecera con registros del MCU

#define RS 0x04        // PTA2: Register Select
#define RW 0x10        // PTA4: Read/Write (se puede atar a GND fisicamente)
#define EN 0x20        // PTA5: Enable
#define DATA_MASK 0xF0 // PTD4-PTD7: 4 lineas de datos

void delayMs(int n);
void LCD_nibble(unsigned char nibble, unsigned char rs);
void LCD_command(unsigned char cmd);
void LCD_data(unsigned char data);
void LCD_init(void);
void LCD_print(char *str);

int main(void) {
    LCD_init();
    while (1) {
        LCD_command(0x01);           // Limpiar pantalla
        delayMs(500);
        LCD_command(0x80);           // Cursor al inicio de linea 1
        LCD_print("Hello world");  // Mostrar mensaje
        delayMs(2000);
    }
}

// Envia un nibble (4 bits) con RS segun rs (0=comando,1=dato)
void LCD_nibble(unsigned char nibble, unsigned char rs) {
    // Prepara lineas de datos, borra bits anteriores
    PTD->PDOR = (PTD->PDOR & ~DATA_MASK) | ((nibble & 0x0F) << 4);
    // Selecciona comando/dato
    if (rs) PTA->PSOR = RS; else PTA->PCOR = RS;
    // RW = 0 (si esta conectado)
    PTA->PCOR = RW;
    // Pulso de enable
    PTA->PSOR = EN;
    delayMs(1);
    PTA->PCOR = EN;
    delayMs(1);
}

// Envía un comando de 8 bits en dos nibbles
void LCD_command(unsigned char cmd) {
    LCD_nibble(cmd >> 4, 0);
    LCD_nibble(cmd & 0x0F, 0);
    // Algunos comandos necesitan mayor retardo
    if (cmd == 0x01 || cmd == 0x02)
        delayMs(2);
}

// Envia un dato (carácter ASCII)
void LCD_data(unsigned char data) {
    LCD_nibble(data >> 4, 1);
    LCD_nibble(data & 0x0F, 1);
}

// Imprime una cadena en el LCD
void LCD_print(char *str) {
    while (*str) {
        LCD_data(*str++);
    }
}

// Inicializacion del LCD en modo 4 bits
void LCD_init(void) {
    // Habilitar reloj a Port D y Port A
    SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK | SIM_SCGC5_PORTA_MASK;
    // Configurar PTD4-PTD7 como GPIO
    for (int i = 4; i <= 7; i++) {
        PORTD->PCR[i] = PORT_PCR_MUX(1);
    }
    PTD->PDDR |= DATA_MASK;
    // Configurar PTA2, PTA4, PTA5 como GPIO
    PORTA->PCR[2] = PORT_PCR_MUX(1);
    PORTA->PCR[4] = PORT_PCR_MUX(1);
    PORTA->PCR[5] = PORT_PCR_MUX(1);
    PTA->PDDR |= RS | RW | EN;
    PTA->PCOR = RW; // RW = 0

    // Secuencia de arranque 4-bit
    delayMs(40);                // Esperar >40 ms tras VCC
    LCD_nibble(0x03, 0);
    delayMs(5);                 // Esperar >4.1 ms
    LCD_nibble(0x03, 0);
    delayMs(1);                 // Esperar >100 ms
    LCD_nibble(0x03, 0);
    delayMs(1);
    LCD_nibble(0x02, 0);        // Poner en modo 4 bits

    // Configurar lineas, entrada de cursor y display
    LCD_command(0x28); // Funcion: 4-bit, 2 lineas, fuente 5x7
    LCD_command(0x06); // Modo de entrada: cursor avanza a la derecha
    LCD_command(0x01); // Limpiar pantalla
    LCD_command(0x0C); // Display ON, cursor OFF
}

// Retraso aproximado en milisegundos (ajustar segun reloj)
void delayMs(int n) {
    volatile int i;
    while (n-- > 0) {
        for (i = 0; i < 7000; i++) __NOP();
    }
}

