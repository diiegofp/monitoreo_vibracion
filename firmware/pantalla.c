#include "stm32f302x8.h"
#include <stdint.h>

/* Prototipos de funciones internas para evitar errores de declaración implícita */
void LCD_CS_LOW(void);
void LCD_CS_HIGH(void);
void LCD_DC_CMD(void);
void LCD_DC_DATA(void);

/**
 * @brief Configuración de SPI3 por registros (Cortex-M4)
 * SCK:  PC10 (AF6)
 * MOSI: PC12 (AF6)
 * CS:   PA8  (Salida Digital)
 * DC:   PA9  (Salida Digital)
 * RES:  PC7  (Salida Digital)
 */
void configuracion_SPI(void){
	/* 1. Habilitación de relojes */
	RCC->AHBENR |= (RCC_AHBENR_GPIOCEN | RCC_AHBENR_GPIOAEN); 
	RCC->APB1ENR |= RCC_APB1ENR_SPI3EN; // SPI3 en bus APB1
	
	/* 2. Configuración de pines para Función Alterna (PC10 y PC12) */
	GPIOC->MODER &= ~((3 << (10 * 2)) | (3 << (12 * 2)));
	GPIOC->MODER |=  ((2 << (10 * 2)) | (2 << (12 * 2))); // Modo AF (10)

	// Selección de AF6 para SPI3 en el registro High de GPIO C
	GPIOC->AFR[1] &= ~((0xF << 8) | (0xF << 16)); 
	GPIOC->AFR[1] |=  ((6 << 8) | (6 << 16));    
	
	/* 3. Salidas digitales de control (CS, DC, RES) */
	// PA8 (CS) y PA9 (DC)
	GPIOA->MODER &= ~((3 << (8 * 2)) | (3 << (9 * 2)));
	GPIOA->MODER |=  ((1 << (8 * 2)) | (1 << (9 * 2))); 
	
	// PC7 (Reset)
	GPIOC->MODER &= ~(3 << (7 * 2));
	GPIOC->MODER |=  (1 << (7 * 2)); 
	
	/* 4. Configuración del periférico SPI3 */
	SPI3->CR1 = 0; // Reset por seguridad
	SPI3->CR1 |= (1 << 9) | (1 << 8) | (1 << 2); // SSM, SSI, Maestro
	SPI3->CR1 |= (0x7 << 3);                     // Baud rate: fPCLK / 16
	SPI3->CR1 &= ~(SPI_CR1_CPOL | SPI_CR1_CPHA); // Modo 0 (Estándar GC9A01)
	
	SPI3->CR2 = 0;
	SPI3->CR2 |= (7 << 8);  // DS = 8 bits (0111)
	SPI3->CR2 |= (1 << 12); // FRXTH (FIFO Threshold 8-bit)
	
	SPI3->CR1 |= (1 << 6);  // SPE = 1 (Habilitar periférico)
}

/**
 * @brief Envío de bajo nivel (puro hardware)
 * Nota: Acceso forzado a 8 bits mediante puntero (__IO uint8_t *)
 */
void envio_SPI (uint8_t dato) {
	while (!(SPI3->SR & SPI_SR_TXE));       // Esperar buffer vacío
	*(__IO uint8_t *)&SPI3->DR = dato;     // Enviar solo 1 byte
}

/**
 * @brief Envío de comandos al controlador de la pantalla
 */
void envio_comando(uint8_t cmnd) {
	LCD_DC_CMD();
	LCD_CS_LOW();
	envio_SPI(cmnd);
	while(SPI3->SR & SPI_SR_BSY);          // Esperar a que el bus se libere
	LCD_CS_HIGH();
}

/**
 * @brief Envío de datos (parámetros o píxeles)
 */
void envio_pantalla (uint8_t dato) {
	LCD_DC_DATA();
	LCD_CS_LOW();
	envio_SPI(dato);
	while(SPI3->SR & SPI_SR_BSY);          // Esperar a que el bus se libere
	LCD_CS_HIGH();
}

/* --- Funciones de manipulación de Pines (Capa de Abstracción) --- */

void LCD_CS_LOW(void)  { GPIOA->ODR &= ~(1 << 8); }
void LCD_CS_HIGH(void) { GPIOA->ODR |=  (1 << 8); }
void LCD_DC_CMD(void)  { GPIOA->ODR &= ~(1 << 9); }
void LCD_DC_DATA(void) { GPIOA->ODR |=  (1 << 9); }

void LCD_RST_LOW(void) { GPIOC->ODR &= ~(1 << 7); }
void LCD_RST_HIGH(void){ GPIOC->ODR |=  (1 << 7); }

// Puentes para la librería externa
void SPI2_Write(uint8_t data){ envio_SPI(data); }

/**
 * @brief Retardo por ciclos de reloj (Aproximado para 8MHz/72MHz)
 */
void delay_ms(uint32_t ms){
	for(uint32_t i = 0; i < ms * 8000; i++);
}