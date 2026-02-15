#ifndef SH1106_H
#define SH1106_H

#include "stm32f302x8.h"
#include <stdint.h>

/* dirección I2C del SH1106 */
#define SH1106_I2C_ADDR     0x3C  // Dirección típica (puede ser 0x3D en algunos casos)

/* comandos del SH1106 */
#define SH1106_CMD_MODE     0x00
#define SH1106_DATA_MODE    0x40

/* dimensiones de la pantalla */
#define SH1106_WIDTH        128
#define SH1106_HEIGHT       64

/* buffer externo */
extern uint8_t sh1106_buffer[1024];

void SH1106_Init(void);
void SH1106_Clear(void);
void SH1106_Display(void);
void SH1106_SetCursor(uint8_t x, uint8_t y);
void SH1106_WriteChar(char ch);
void SH1106_WriteString(char* str);
void SH1106_WriteNumber(int num);
void SH1106_WriteFloat(float value);
void SH1106_TestPattern(void);

#endif /* SH1106_H */