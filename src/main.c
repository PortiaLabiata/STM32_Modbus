#include "main.h"
#include "core/system.h"
#include "core/types.h"
#include "core/interrupts.h"

#include "mb.h"
#include "port.h"

int main(void) {
    Clock_Config();
    GPIO_Config();
    TIM3_Config();

    xMBPortTimersInit(7);
    vMBPortTimersEnable();
    
    while (1) {

    }
}