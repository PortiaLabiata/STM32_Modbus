#include "core/interrupts.h"
#include "core/system.h"

void SysTick_Handler(void) {
    _current_ticks++;
}