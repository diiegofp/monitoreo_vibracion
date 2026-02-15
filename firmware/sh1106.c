#include "sh1106.h"
#include "utils.h"
#include <string.h>

#include "sh1106.h"
#include "utils.h"
#include <string.h>

uint8_t sh1106_buffer[SH1106_WIDTH * SH1106_HEIGHT / 8];

/* Fuente 5x7 para caracteres ASCII básicos */
static const uint8_t font_5x7[][5] = {
    {0x00, 0x00, 0x00, 0x00, 0x00}, // 32: espacio
    {0x00, 0x00, 0x5F, 0x00, 0x00}, // 33: !
    {0x00, 0x07, 0x00, 0x07, 0x00}, // 34: "
    {0x14, 0x7F, 0x14, 0x7F, 0x14}, // 35: #
    {0x24, 0x2A, 0x7F, 0x2A, 0x12}, // 36: $
    {0x23, 0x13, 0x08, 0x64, 0x62}, // 37: %
    {0x36, 0x49, 0x56, 0x20, 0x50}, // 38: &
    {0x00, 0x08, 0x07, 0x03, 0x00}, // 39: '
    {0x00, 0x1C, 0x22, 0x41, 0x00}, // 40: (
    {0x00, 0x41, 0x22, 0x1C, 0x00}, // 41: )
    {0x2A, 0x1C, 0x7F, 0x1C, 0x2A}, // 42: *
    {0x08, 0x08, 0x3E, 0x08, 0x08}, // 43: +
    {0x00, 0x80, 0x70, 0x30, 0x00}, // 44: ,
    {0x08, 0x08, 0x08, 0x08, 0x08}, // 45: -
    {0x00, 0x00, 0x60, 0x60, 0x00}, // 46: .
    {0x20, 0x10, 0x08, 0x04, 0x02}, // 47: /
    {0x3E, 0x51, 0x49, 0x45, 0x3E}, // 48: 0
    {0x00, 0x42, 0x7F, 0x40, 0x00}, // 49: 1
    {0x72, 0x49, 0x49, 0x49, 0x46}, // 50: 2
    {0x21, 0x41, 0x49, 0x4D, 0x33}, // 51: 3
    {0x18, 0x14, 0x12, 0x7F, 0x10}, // 52: 4
    {0x27, 0x45, 0x45, 0x45, 0x39}, // 53: 5
    {0x3C, 0x4A, 0x49, 0x49, 0x31}, // 54: 6
    {0x41, 0x21, 0x11, 0x09, 0x07}, // 55: 7
    {0x36, 0x49, 0x49, 0x49, 0x36}, // 56: 8
    {0x46, 0x49, 0x49, 0x29, 0x1E}, // 57: 9
    {0x00, 0x00, 0x14, 0x00, 0x00}, // 58: :
    {0x00, 0x40, 0x34, 0x00, 0x00}, // 59: ;
    {0x00, 0x08, 0x14, 0x22, 0x41}, // 60: 
    {0x14, 0x14, 0x14, 0x14, 0x14}, // 61: =
    {0x00, 0x41, 0x22, 0x14, 0x08}, // 62: >
    {0x02, 0x01, 0x59, 0x09, 0x06}, // 63: ?
    {0x3E, 0x41, 0x5D, 0x59, 0x4E}, // 64: @
    {0x7C, 0x12, 0x11, 0x12, 0x7C}, // 65: A
    {0x7F, 0x49, 0x49, 0x49, 0x36}, // 66: B
    {0x3E, 0x41, 0x41, 0x41, 0x22}, // 67: C
    {0x7F, 0x41, 0x41, 0x41, 0x3E}, // 68: D
    {0x7F, 0x49, 0x49, 0x49, 0x41}, // 69: E
    {0x7F, 0x09, 0x09, 0x09, 0x01}, // 70: F
    {0x3E, 0x41, 0x41, 0x51, 0x73}, // 71: G
    {0x7F, 0x08, 0x08, 0x08, 0x7F}, // 72: H
    {0x00, 0x41, 0x7F, 0x41, 0x00}, // 73: I
    {0x20, 0x40, 0x41, 0x3F, 0x01}, // 74: J
    {0x7F, 0x08, 0x14, 0x22, 0x41}, // 75: K
    {0x7F, 0x40, 0x40, 0x40, 0x40}, // 76: L
    {0x7F, 0x02, 0x1C, 0x02, 0x7F}, // 77: M
    {0x7F, 0x04, 0x08, 0x10, 0x7F}, // 78: N
    {0x3E, 0x41, 0x41, 0x41, 0x3E}, // 79: O
    {0x7F, 0x09, 0x09, 0x09, 0x06}, // 80: P
    {0x3E, 0x41, 0x51, 0x21, 0x5E}, // 81: Q
    {0x7F, 0x09, 0x19, 0x29, 0x46}, // 82: R
    {0x26, 0x49, 0x49, 0x49, 0x32}, // 83: S
    {0x03, 0x01, 0x7F, 0x01, 0x03}, // 84: T
    {0x3F, 0x40, 0x40, 0x40, 0x3F}, // 85: U
    {0x1F, 0x20, 0x40, 0x20, 0x1F}, // 86: V
    {0x3F, 0x40, 0x38, 0x40, 0x3F}, // 87: W
    {0x63, 0x14, 0x08, 0x14, 0x63}, // 88: X
    {0x03, 0x04, 0x78, 0x04, 0x03}, // 89: Y
    {0x61, 0x59, 0x49, 0x4D, 0x43}, // 90: Z
};

/* Variables de cursor */
static uint8_t cursor_x = 0;
static uint8_t cursor_y = 0;

static void SH1106_WriteCommand(uint8_t cmd) {
    I2C1->CR2 = 0;
    I2C1->CR2 |= (SH1106_I2C_ADDR << 1);  
    I2C1->CR2 |= (2 << 16);
    I2C1->CR2 |= (1 << 25);
    I2C1->CR2 |= (1 << 13);
    
    while(!(I2C1->ISR & I2C_ISR_TXIS));
    I2C1->TXDR = 0x00;
    
    while(!(I2C1->ISR & I2C_ISR_TXIS));
    I2C1->TXDR = cmd;
    
    while(!(I2C1->ISR & I2C_ISR_STOPF));
    I2C1->ICR = I2C_ICR_STOPCF;
}

static void SH1106_WriteData(uint8_t data) {
    I2C1->CR2 = 0;
    I2C1->CR2 |= (SH1106_I2C_ADDR << 1);  
    I2C1->CR2 |= (2 << 16);    
    I2C1->CR2 |= (1 << 25);
    I2C1->CR2 |= (1 << 13);
    
    while(!(I2C1->ISR & I2C_ISR_TXIS));
    I2C1->TXDR = 0x40;
    
    while(!(I2C1->ISR & I2C_ISR_TXIS));
    I2C1->TXDR = data;
    
    while(!(I2C1->ISR & I2C_ISR_STOPF));
    I2C1->ICR = I2C_ICR_STOPCF;
}

void SH1106_Init(void) {
    delay_ms(100);
    
    SH1106_WriteCommand(0xAE);
    SH1106_WriteCommand(0xD5);
    SH1106_WriteCommand(0x80);
    SH1106_WriteCommand(0xA8);
    SH1106_WriteCommand(0x3F);
    SH1106_WriteCommand(0xD3);
    SH1106_WriteCommand(0x00);
    SH1106_WriteCommand(0x40);
    SH1106_WriteCommand(0x8D);
    SH1106_WriteCommand(0x14);
    SH1106_WriteCommand(0x20);
    SH1106_WriteCommand(0x00);
    SH1106_WriteCommand(0xA1);
    SH1106_WriteCommand(0xC8);
    SH1106_WriteCommand(0xDA);
    SH1106_WriteCommand(0x12);
    SH1106_WriteCommand(0x81);
    SH1106_WriteCommand(0x7F);  
    SH1106_WriteCommand(0xD9);
    SH1106_WriteCommand(0xF1);
    SH1106_WriteCommand(0xDB);
    SH1106_WriteCommand(0x40);
    SH1106_WriteCommand(0xA4);
    SH1106_WriteCommand(0xA6);
    
    SH1106_Clear();
    
    SH1106_WriteCommand(0xAF);
    delay_ms(100);
}

void SH1106_Clear(void) {
    memset(sh1106_buffer, 0x00, sizeof(sh1106_buffer));
    SH1106_Display();
}

void SH1106_Display(void) {
    for (uint8_t page = 0; page < 8; page++) {
        SH1106_WriteCommand(0xB0 + page);
        SH1106_WriteCommand(0x00);
        SH1106_WriteCommand(0x10);
        
        for (uint8_t col = 0; col < 128; col++) {
            SH1106_WriteData(sh1106_buffer[page * 128 + col]);
        }
    }
}

void SH1106_TestPattern(void) {
    memset(sh1106_buffer, 0xAA, sizeof(sh1106_buffer));
    SH1106_Display();
    delay_ms(2000);
    
    memset(sh1106_buffer, 0xFF, sizeof(sh1106_buffer));
    SH1106_Display();
    delay_ms(2000);
    
    memset(sh1106_buffer, 0x00, sizeof(sh1106_buffer));
    for (uint8_t i = 0; i < 128; i++) {
        sh1106_buffer[i] = 0xFF;
        sh1106_buffer[7*128 + i] = 0xFF;
    }
    for (uint8_t page = 0; page < 8; page++) {
        sh1106_buffer[page * 128] = 0xFF;
        sh1106_buffer[page * 128 + 127] = 0xFF;
    }
    SH1106_Display();
}

/* Establecer posición del cursor */
void SH1106_SetCursor(uint8_t x, uint8_t y) {
    cursor_x = x;
    cursor_y = y;
}

/* Escribir un carácter en el buffer */
void SH1106_WriteChar(char ch) {
    uint8_t i;
    
    // Solo caracteres imprimibles
    if(ch < 32 || ch > 90) {
        ch = 32; // Espacio por defecto
    }
    
    // Obtener índice de la fuente
    uint8_t font_index = ch - 32;
    
    // Dibujar cada columna del carácter
    for(i = 0; i < 5; i++) {
        if(cursor_x < 128) {
            // Calcular posición en el buffer
            uint16_t buffer_pos = cursor_y * 128 + cursor_x;
            
            if(buffer_pos < 1024) {
                sh1106_buffer[buffer_pos] = font_5x7[font_index][i];
            }
            cursor_x++;
        }
    }
    
    // Espacio entre caracteres
    if(cursor_x < 128) {
        sh1106_buffer[cursor_y * 128 + cursor_x] = 0x00;
        cursor_x++;
    }
}

/* Escribir una cadena */
void SH1106_WriteString(char* str) {
    while(*str) {
        SH1106_WriteChar(*str++);
    }
}

/* Convertir número a string y mostrarlo */
void SH1106_WriteNumber(int num) {
    char buffer[12];
    int i = 0;
    int is_negative = 0;
    
    // Manejar cero
    if(num == 0) {
        SH1106_WriteChar('0');
        return;
    }
    
    // Manejar negativos
    if(num < 0) {
        is_negative = 1;
        num = -num;
    }
    
    // Convertir a string (invertido)
    while(num > 0) {
        buffer[i++] = (num % 10) + '0';
        num /= 10;
    }
    
    // Agregar signo negativo si es necesario
    if(is_negative) {
        buffer[i++] = '-';
    }
    
    // Escribir al revés
    while(i > 0) {
        SH1106_WriteChar(buffer[--i]);
    }
}

/* Escribir número flotante con 1 decimal */
void SH1106_WriteFloat(float value) {
    int entero = (int)value;
    int decimal = (int)((value - entero) * 10);
    
    if(decimal < 0) decimal = -decimal;
    
    SH1106_WriteNumber(entero);
    SH1106_WriteChar('.');
    SH1106_WriteChar('0' + decimal);
}