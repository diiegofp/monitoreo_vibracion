#ifndef PANTALLA_H
#define PANTALLA_H

#include <stdint.h>

void LCD_CS_LOW(void);
void LCD_CS_HIGH(void);
void LCD_DC_CMD(void);
void LCD_DC_DATA(void);
void LCD_RST_LOW(void);
void LCD_RST_HIGH(void);
void SPI2_Write(uint8_t data);
void delay_ms(uint32_t ms);

void configuracion_SPI(void);
void envio_SPI (uint8_t dato);
void envio_comando(uint8_t cmnd);
void envio_pantalla (uint8_t dato);
void delay_ms(uint32_t ms);

#endif