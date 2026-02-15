#include "stm32f302x8.h"
#include "fsm.h"
#include <stdint.h>

int main(void) {
    //instacnai maquina de estados
    FSM_Monitor_t miMonitor;
    
		//inicio
    FSM_Init(&miMonitor);
    
    //ciclo infinito
    while(1) {
        FSM_Run(&miMonitor);
    }
}