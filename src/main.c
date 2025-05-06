#include "main.h"
#include "core/system.h"
#include "core/types.h"

int main(void) {
    Clock_Config();
    GPIO_Config();
    
    while (1) ;
}