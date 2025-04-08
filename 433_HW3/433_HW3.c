#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
 
#define GPIO_WATCH_PIN 2  //Button is Connected to Pin 2 
#define LED_PIN 1 //IDR What Pin IT Was TBH 


int main()
{
    stdio_init_all(); //enable the USB Communication
    gpio_init(GPIO_WATCH_PIN); //Watching the Button 
    gpio_set_dir(GPIO_WATCH_PIN, GPIO_IN);    
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);    
    adc_init(); // init the adc module
    adc_gpio_init(26); // set ADC0 pin to be adc input instead of GPIO
    adc_select_input(0); // select to read from ADC0

    while (!stdio_usb_connected()) { //waiting for the screen to open the port 
        sleep_ms(100);
    }
    gpio_put(LED_PIN,1);
    while(1){       
        while(gpio_get(GPIO_WATCH_PIN) = 0){
            // hang out untill the button is pressed 
        }
        gpio_put(LED_PIN, 0); // Turning Off the 
        char num_samples[50];
        scanf("%d",num_samples);
        uint16_t result = adc_read(); //reading the adc 
    }
   return 0;
}
