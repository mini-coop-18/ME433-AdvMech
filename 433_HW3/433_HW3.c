#include <stdio.h>
#include "pico/stdlib.h"
#include 

int main()
{
    stdio_init_all(); //enable the USB Communication 
    while (!stdio_usb_connected()) { //waiting for the screen to open the port 
        sleep_ms(100);
    }
    while (true) {
        printf("Hello, world!\n");
        sleep_ms(1000);
    }
}
