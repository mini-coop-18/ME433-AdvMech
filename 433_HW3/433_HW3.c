#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
 
#define GPIO_WATCH_PIN 2  //Button is Connected to Pin 2 
#define LED_PIN 16 //IDR What Pin IT Was TBH 
#define POT_PIN 26
#define POT_PIN_A 0


int main()
{
    stdio_init_all(); //enable the USB Communication
    gpio_init(GPIO_WATCH_PIN); //Watching the Button 
    gpio_set_dir(GPIO_WATCH_PIN, GPIO_IN);    
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);    
    adc_init(); // init the adc module
    adc_gpio_init(POT_PIN); // set ADC0 pin to be adc input instead of GPIO
    adc_select_input(POT_PIN_A); // select to read from ADC0

    while (!stdio_usb_connected()) { //waiting for the screen to open the port 
        sleep_ms(100);
    }
    gpio_put(LED_PIN,0);
    printf("Let's Go! \r\n");
    while(1){    
        gpio_put(LED_PIN,0);   
        while(gpio_get(GPIO_WATCH_PIN) == 1){
            ;// hang out untill the button is pressed 
        }
        gpio_put(LED_PIN, 1); // Turning Off the Lx
        int num_samples;
        char message[100];
        printf("How Many Samples Would You Like to Take? \r\n");
        scanf("%d", &num_samples); 
        printf("Ok, %d Samples, Coming Right Up \r\n", num_samples);
        for (int i = 1; i<num_samples+1; i++){
            uint16_t result = adc_read(); //reading the adc 
            float result_v = (float)3.3*result/4095; //convert to volts
            printf("Sample %d: ADC is %f\n\r", i, result_v);
            sleep_ms(10); //wait for 10ms for 100 Hz 
        }
        printf("All Done!\n\r Press the Button to Go Again! \n\r");
    }
   return 0;
}
