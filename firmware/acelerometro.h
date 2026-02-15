#ifndef ACELEROMETRO_H
#define ACELEROMETRO_H

#include <stdint.h>

/* direcciones y registros del LIS3DH */
#define DIRECCION_ACEL  0x18
#define WHO_AM_I        0x0F
#define CTRL_REG1       0x20
#define CTRL_REG4       0x23
#define STATUS_REG      0x27
#define OUT_X_L         0x28
#define OUT_X_H         0x29
#define OUT_Y_L         0x2A
#define OUT_Y_H         0x2B
#define OUT_Z_L         0x2C
#define OUT_Z_H         0x2D

void configuracion_I2C(void);
void I2C1_WR(uint8_t direccion_esclavo, uint8_t direccion_reg, uint8_t dato);
uint8_t I2C1_RR(uint8_t direccion_esclavo, uint8_t direccion_reg);
void LIS3DH_Init(void);
void LIS3DH_LeerEjes(int16_t* x, int16_t* y, int16_t* z);
float LIS3DH_CalcularRMS(int16_t x, int16_t y, int16_t z);

#endif