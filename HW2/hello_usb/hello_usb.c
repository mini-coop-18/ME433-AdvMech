/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

 #include <stdio.h>
 #include "pico/stdlib.h"
 #include "hardware/gpio.h"
 
 #define GPIO_WATCH_PIN 2 

 void gpio_callback(uint gpio, uint32_t events) {
    static int count = 0;
    char message[50];
    printf("Button Count is %d\n", count);
    count++;
}

int main() {
    stdio_init_all();
    gpio_init(GPIO_WATCH_PIN);
    gpio_set_irq_enabled_with_callback(GPIO_WATCH_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
    while (true) {
        // printf("Hi Grace\n");
        // sleep_ms(1000);
    }
}
