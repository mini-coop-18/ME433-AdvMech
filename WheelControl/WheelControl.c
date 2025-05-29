#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#define LEFT_WHEEL 17
#define LEFT_BLINKER 15

int main()
{
    stdio_init_all();
    gpio_init(LEFT_WHEEL);  
    gpio_set_dir(LEFT_WHEEL, 1); 
    gpio_put(LEFT_WHEEL, 1);

    gpio_init(LEFT_BLINKER);  
    gpio_set_dir(LEFT_BLINKER, 1); 
    gpio_put(LEFT_BLINKER, 1);
    while (true) {
    
    }
}
