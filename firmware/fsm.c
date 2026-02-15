#include "fsm.h"
#include "stm32f302x8.h"
#include "sh1106.h"
#include "acelerometro.h"
#include "utils.h"
#include <string.h>

//variables datos del sentor
static int16_t datosX, datosY, datosZ;
static float valorRms;
static uint32_t contadorActualizaciones = 0;


static void estadoInicializacion(FSM_Monitor_t* fsm);
static void estadoLectura(FSM_Monitor_t* fsm);
static void estadoProcesamiento(FSM_Monitor_t* fsm);
static void estadoVisualizacion(FSM_Monitor_t* fsm);
static void estadoEspera(FSM_Monitor_t* fsm);

//inicio de la maquina de estaods
void FSM_Init(FSM_Monitor_t* fsm) {
    fsm->estadoActual = FSM_INICIALIZACION;
    fsm->estadoAnterior = FSM_INICIALIZACION;
    fsm->tiempoUltimoCambio = 0;
    fsm->inicializacionCompleta = 0;
    
    datosX = 0;
    datosY = 0;
    datosZ = 0;
    valorRms = 0.0f;
    contadorActualizaciones = 0;
}

//switch - case para la fsm

void FSM_Run(FSM_Monitor_t* fsm) {
    switch(fsm->estadoActual) {
        case FSM_INICIALIZACION:
            estadoInicializacion(fsm);
            break;
            
        case FSM_LECTURA:
            estadoLectura(fsm);
            break;
            
        case FSM_PROCESAMIENTO:
            estadoProcesamiento(fsm);
            break;
            
        case FSM_VISUALIZACION:
            estadoVisualizacion(fsm);
            break;
            
        case FSM_ESPERA:
            estadoEspera(fsm);
            break;
            
        default:
            // si se traba regresa al inicio
            fsm->estadoActual = FSM_INICIALIZACION;
            break;
    }
}

//puras configuraciones
static void estadoInicializacion(FSM_Monitor_t* fsm) {
    // se activa la FPU
    SCB->CPACR |= ((3 << 20) | (3 << 22));  
    

    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;  
    
    configuracion_I2C();
    delay_ms(100);
    
    SH1106_Init();
    SH1106_Clear();
    
    // inicio pantalla
    SH1106_SetCursor(10, 2);
    SH1106_WriteString("SISTEMA DE");
    SH1106_SetCursor(10, 4);
    SH1106_WriteString("MONITOREO");
    SH1106_Display();
    delay_ms(2000);
    
    LIS3DH_Init();
    delay_ms(100);
    
    fsm->inicializacionCompleta = 1;
    
    // siguiente estado
    fsm->estadoAnterior = fsm->estadoActual;
    fsm->estadoActual = FSM_LECTURA;
}

//lectura de ejes
static void estadoLectura(FSM_Monitor_t* fsm) {
    // Se traen los 6 registros de los ejes
    LIS3DH_LeerEjes(&datosX, &datosY, &datosZ);
    
    fsm->estadoAnterior = fsm->estadoActual;
    fsm->estadoActual = FSM_PROCESAMIENTO;
}

//calculo del valor RMS
static void estadoProcesamiento(FSM_Monitor_t* fsm) {
    // Sacar la magnitud usando la funcion que hicimos
    valorRms = LIS3DH_CalcularRMS(datosX, datosY, datosZ);
    
    contadorActualizaciones++;
    
    fsm->estadoAnterior = fsm->estadoActual;
    fsm->estadoActual = FSM_VISUALIZACION;
}

//muestreo de datos en pantalla
static void estadoVisualizacion(FSM_Monitor_t* fsm) {
   
    for(int i = 0; i < 128; i++) {
        sh1106_buffer[0 * 128 + i] = 0x00;  
        sh1106_buffer[2 * 128 + i] = 0x00;  
        sh1106_buffer[4 * 128 + i] = 0x00;  
        sh1106_buffer[6 * 128 + i] = 0x00;  
    }
    
    // escribimos el RMS
    SH1106_SetCursor(2, 0);
    SH1106_WriteString("RMS: ");
    SH1106_WriteFloat(valorRms);
    SH1106_WriteString(" G");
    
    // comparacion con umbrales
    SH1106_SetCursor(2, 2);
    SH1106_WriteString("ESTADO: ");
    if(valorRms < 18.0f) {
        SH1106_WriteString("OK   ");
    } else if(valorRms < 24.0f) {
        SH1106_WriteString("MEDIO");
    } else {
        SH1106_WriteString("ALTO ");
    }
    
    // colocacion de ejes
    SH1106_SetCursor(2, 4);
    SH1106_WriteString("X:");
    SH1106_WriteNumber(datosX / 1000);
    SH1106_WriteString(" Y:");
    SH1106_WriteNumber(datosY / 1000);
    
    SH1106_SetCursor(2, 6);
    SH1106_WriteString("Z:");
    SH1106_WriteNumber(datosZ / 1000);
    
    //despligue de todo
    SH1106_Display();
    
    fsm->estadoAnterior = fsm->estadoActual;
    fsm->estadoActual = FSM_ESPERA;
}


static void estadoEspera(FSM_Monitor_t* fsm) {
  
    delay_ms(200);
    
    fsm->estadoAnterior = fsm->estadoActual;
    fsm->estadoActual = FSM_LECTURA;
}


const char* FSM_GetEstadoString(FSM_Estado_t estado) {
    switch(estado) {
        case FSM_INICIALIZACION:  return "INICIALIZACION";
        case FSM_LECTURA:         return "LECTURA";
        case FSM_PROCESAMIENTO:   return "PROCESAMIENTO";
        case FSM_VISUALIZACION:   return "VISUALIZACION";
        case FSM_ESPERA:          return "ESPERA";
        default:                  return "DESCONOCIDO";
    }
}