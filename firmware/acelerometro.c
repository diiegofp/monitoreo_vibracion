#include "stm32f302x8.h"
#include <stdint.h>
#include "acelerometro.h"
#include "utils.h"

void configuracion_I2C(void) {
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
    
    for(volatile int i = 0; i < 1000; i++);
    
    RCC->APB1RSTR |= RCC_APB1RSTR_I2C1RST;
    RCC->APB1RSTR &= ~RCC_APB1RSTR_I2C1RST;
    
    GPIOB->MODER &= ~(0xF << 16);
    GPIOB->MODER |= (0xA << 16);
    
    GPIOB->OTYPER |= (3 << 8);
    GPIOB->OSPEEDR |= (0xF << 16);
    
    GPIOB->PUPDR &= ~(0xF << 16);
    GPIOB->PUPDR |= (0x5 << 16);
    
    GPIOB->AFR[1] &= ~(0xFF);
    GPIOB->AFR[1] |= (0x44);
    
    I2C1->CR1 = 0;
    
		I2C1->TIMINGR = 0x2000090E;
    I2C1->CR1 |= I2C_CR1_PE;
}

// funcion para configurar registros del accelerometro
void I2C1_WR(uint8_t direccion_esclavo, uint8_t direccion_reg, uint8_t dato) {
    I2C1->CR2 = 0;
    I2C1->CR2 |= (direccion_esclavo << 1);
    I2C1->CR2 |= (2 << 16);      // numero de bytes
    I2C1->CR2 |= (1 << 25);      // autoend
    I2C1->CR2 &= ~(1 << 10);     // write
    I2C1->CR2 |= (1 << 13);      // start 
    
    //direccion registro envio
    while(!(I2C1->ISR & I2C_ISR_TXIS));
    I2C1->TXDR = direccion_reg;
    
    //envio datos
    while(!(I2C1->ISR & I2C_ISR_TXIS));
    I2C1->TXDR = dato;
    
    // esperamos al stop
    while(!(I2C1->ISR & I2C_ISR_STOPF));
    I2C1->ICR = I2C_ICR_STOPCF;
}


uint8_t I2C1_RR(uint8_t direccion_esclavo, uint8_t direccion_reg) {
    uint8_t dato_leido = 0;
    
   
    I2C1->CR2 = 0;
    I2C1->CR2 |= (direccion_esclavo << 1);
    I2C1->CR2 |= (1 << 16);      // numero de bytes
    I2C1->CR2 &= ~(1 << 25);     // autoend
    I2C1->CR2 &= ~(1 << 10);     // write
    I2C1->CR2 |= (1 << 13);      // start
    
    // esperar a txis
    while(!(I2C1->ISR & I2C_ISR_TXIS));
    
    I2C1->TXDR = direccion_reg;
    
    // esperamos a que haya sido completa la trnasferencia
    while(!(I2C1->ISR & I2C_ISR_TC));
    

    I2C1->CR2 &= ~(0xFF << 16);  //clear
    I2C1->CR2 |= (1 << 16);      // 1 byte
    I2C1->CR2 |= (1 << 25);      
    I2C1->CR2 |= (1 << 10);     
    I2C1->CR2 |= (1 << 13);     
    
    // esperar a RXNE
    while(!(I2C1->ISR & I2C_ISR_RXNE));
    
   
    dato_leido = I2C1->RXDR;

    while(!(I2C1->ISR & I2C_ISR_STOPF));
    
    // limpiamos bandera
    I2C1->ICR = I2C_ICR_STOPCF;
    
    return dato_leido;
}


void LIS3DH_Init(void) {
    //verificacion direccion
    (void)I2C1_RR(DIRECCION_ACEL, WHO_AM_I); 
    
    // ejes habilitados
    I2C1_WR(DIRECCION_ACEL, CTRL_REG1, 0x77);
    
    // +4g configurado
    I2C1_WR(DIRECCION_ACEL, CTRL_REG4, 0x18);
}

/* lectrua de los 3 ejes del acelerómetro */
void LIS3DH_LeerEjes(int16_t* x, int16_t* y, int16_t* z) {
    uint8_t xl, xh, yl, yh, zl, zh;
    
    //leemos los 6 bytes
    xl = I2C1_RR(DIRECCION_ACEL, OUT_X_L);
    xh = I2C1_RR(DIRECCION_ACEL, OUT_X_H);
    yl = I2C1_RR(DIRECCION_ACEL, OUT_Y_L);
    yh = I2C1_RR(DIRECCION_ACEL, OUT_Y_H);
    zl = I2C1_RR(DIRECCION_ACEL, OUT_Z_L);
    zh = I2C1_RR(DIRECCION_ACEL, OUT_Z_H);
    
    //combinacion ya que son complemento a 2 nos interesa el signo
    *x = (int16_t)((xh << 8) | xl);
    *y = (int16_t)((yh << 8) | yl);
    *z = (int16_t)((zh << 8) | zl);
}


float LIS3DH_CalcularRMS(int16_t x, int16_t y, int16_t z) {
    float x_g = (float)x * 0.002f;
    float y_g = (float)y * 0.002f;
    float z_g = (float)z * 0.002f;
    
    float suma = (x_g * x_g) + (y_g * y_g) + (z_g * z_g);
    
    //raiz cuadrada por aproximacion
    if(suma < 0.0001f) return 0.0f;
    
    float resultado = suma;
    for(int i = 0; i < 10; i++) {
        resultado = 0.5f * (resultado + suma / resultado);
    }
    
    return resultado;
}
