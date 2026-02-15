#ifndef FSM_MONITOR_H
#define FSM_MONITOR_H

#include <stdint.h>

/* estadosss */
typedef enum {
    FSM_INICIALIZACION,     // Para configurar todo (I2C, FPU, OLED, sensor)
    FSM_LECTURA,            // Leer los datos del acelerometro LIS3DH
    FSM_PROCESAMIENTO,      // Hacer los calculos con la FPU
    FSM_VISUALIZACION,      // Mandar los datos a la OLED
    FSM_ESPERA              // Tiempo entre cada lectura
} FSM_Estado_t;

/* control de la FSM*/
typedef struct {
    FSM_Estado_t estadoActual;
    FSM_Estado_t estadoAnterior;
    uint32_t tiempoUltimoCambio;
    uint8_t inicializacionCompleta;
} FSM_Monitor_t;

void FSM_Init(FSM_Monitor_t* fsm);
void FSM_Run(FSM_Monitor_t* fsm);
const char* FSM_GetEstadoString(FSM_Estado_t estado);

#endif