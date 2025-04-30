/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/adc.h"

#define LED_PIN 15 
#define POT_PIN 26
#define POT_PIN_A 0


#define FLAG_VALUE_to_1 123
#define FLAG_VALUE_to_0 321

volatile int message_0_to_1;
volatile int message_1_to_0;

int pico_led_init(void) {
    #if defined(PICO_DEFAULT_LED_PIN)
        // A device like Pico that uses a GPIO for the LED will define PICO_DEFAULT_LED_PIN
        // so we can use normal GPIO functionality to turn the led on and off
        gpio_init(PICO_DEFAULT_LED_PIN);
        gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
        return PICO_OK;
    #elif defined(CYW43_WL_GPIO_LED_PIN)
        // For Pico W devices we need to initialise the driver etc
        return cyw43_arch_init();
    #endif
    }

void pico_set_led(bool led_on) {
    #if defined(PICO_DEFAULT_LED_PIN)
        // Just set the GPIO on or off
        gpio_put(PICO_DEFAULT_LED_PIN, led_on);
    #elif defined(CYW43_WL_GPIO_LED_PIN)
        // Ask the wifi "driver" to set the GPIO on or off
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, led_on);
    #endif
    }


void core1_entry() {
    pico_led_init();
    adc_init(); // init the adc module
    adc_gpio_init(POT_PIN); // set ADC0 pin to be adc input instead of GPIO
    adc_select_input(POT_PIN_A); // select to read from ADC0

    while (1){
    uint32_t g = multicore_fifo_pop_blocking(); //Pull the Message from for 1 

    if (g != FLAG_VALUE_to_1) {
        //printf("Hmm, that's not right on core 1!\n");
        //printf ("g is %d \n\r", g);
    }
    else
        if (message_0_to_1 == 0){
            //printf("Look You Sent a 0 \n");
            multicore_fifo_push_blocking(FLAG_VALUE_to_0); //I'm Ready to Go to Core 1 
            message_0_to_1 = 3;
            uint16_t result = adc_read(); //reading the adc 
            float result_v = (float)3.3*result/4095; //convert to volts
            printf("ADC is %f\n\r", result_v);
            sleep_ms(10); //wait for 10ms for 100 Hz 
        }
        if (message_0_to_1 == 1){
            //printf("Look You Sent a 1 \n") ;
            multicore_fifo_push_blocking(FLAG_VALUE_to_0); //I'm Ready to Go to Core 1 
            message_0_to_1 = 3;
            pico_set_led(false);
        }
        if (message_0_to_1 == 2){
            //printf("Look You Sent a 2\n");
            multicore_fifo_push_blocking(FLAG_VALUE_to_0); //I'm Ready to Go to Core 1 
            message_0_to_1 = 3;
            pico_set_led(true);
        }
    }
}

int main() {
    stdio_init_all();

    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }
    printf("Hello, multicore!\n");
    multicore_launch_core1(core1_entry); //enables Core 1

    while( 1){
        printf("Hello - What would you Like to Do? 0, 1, or 2? \n\r");    
        scanf("%d", &message_0_to_1);
        printf("Great! Sending %d to Core 1 \n\r", message_0_to_1);
        multicore_fifo_push_blocking(FLAG_VALUE_to_1);

        uint32_t g = multicore_fifo_pop_blocking();
        while (g != FLAG_VALUE_to_0){
            //printf("Hmm, that's not right on core 0!\n");
        }
        printf("All Done\n\r");
        }
    /// \end::setup_multicore[]
}
