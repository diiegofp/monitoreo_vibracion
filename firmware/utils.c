#include "utils.h"
#include "stm32f302x8.h"

void delay_ms(uint32_t ms) {
    for(uint32_t i = 0; i < ms * 8000; i++) {
        __NOP();
    }
}