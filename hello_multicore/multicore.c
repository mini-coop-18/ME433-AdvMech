/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"

#include LED_PIN 15 

#define FLAG_VALUE 123

volatile int message_0_to_1
volatile int message_1_to_0 


void core1_entry() {
    multicore_fifo_push_blocking(FLAG_VALUE); //I'm Ready to Go to Core 1 
    uint32_t g = multicore_fifo_pop_blocking(); //Pull the Message from for 1 

    if (g != FLAG_VALUE)
        printf("Hmm, that's not right on core 1!\n");
    else
        if (message_0_to_1 == 0){
            printf("Look You Sent a 0")
            //read voltage
        }
        if (message_0_to_1 == 1){
            printf("Look You Sent a 1") 
            //turn on lx 
        }
        if (message_0_to_1 == 2){
            printf("Look You Sent a 2")
            //turn off lx 
        }

    while (1)
        tight_loop_contents();
}

int main() {
    stdio_init_all();

    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }
    printf("Hello, multicore!\n");

    multicore_launch_core1(core1_entry); //enables Core 1

    printf("Hello - What would you Like to Do? 0, 1, or 2? \n\r")
    scanf("%d", &message_0_to_1)
    multicore_fifo_push_blocking(FLAG_VALUE);
    uint32_t g = multicore_fifo_pop_blocking();
    if (g != FLAG_VALUE)
        printf("Hmm, that's not right on core 0!\n");
    else {
        multicore_fifo_push_blocking(FLAG_VALUE);
        printf("It's all gone well on core 0!");
    }

    /// \end::setup_multicore[]
}
