/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "ws2812.pio.h"
#include "hardware/pwm.h"

/**
 * NOTE:
 *  Take into consideration if your WS2812 is a RGB or RGBW variant.
 *
 *  If it is RGBW, you need to set IS_RGBW to true and provide 4 bytes per 
 *  pixel (Red, Green, Blue, White) and use urgbw_u32().
 *
 *  If it is RGB, set IS_RGBW to false and provide 3 bytes per pixel (Red,
 *  Green, Blue) and use urgb_u32().
 *
 *  When RGBW is used with urgb_u32(), the White channel will be ignored (off).
 *
 */
#define IS_RGBW false
#define NUM_PIXELS 4
#define LIST_LEN 360
#define LEDPin 14

#ifdef PICO_DEFAULT_WS2812_PIN
#define WS2812_PIN PICO_DEFAULT_WS2812_PIN
#else
// default to pin 2 if the board doesn't have a default WS2812 pin defined
#define WS2812_PIN 13
#endif

// Check the pin is compatible with the platform
#if WS2812_PIN >= NUM_BANK0_GPIOS
#error Attempting to use a pin>=32 on a platform that does not support it
#endif

// link three 8bit colors together
typedef struct {
    unsigned char g;
    unsigned char r;
    unsigned char b;
} wsColor; 

static volatile wsColor Rainbow[LIST_LEN]; // waveforms
wsColor HSBtoRGB(float hue, float sat, float brightness);

void make_rainbow(void){
    float c = 0;
    for(int i = 0; i<LIST_LEN; i++){
        float hue = c;
        printf("%f\n\r", hue);
        wsColor color;
        color = HSBtoRGB(hue, 1,.1);
        Rainbow[i].g = color.g;
        Rainbow[i].r = color.r;
        Rainbow[i].b = color.g;
        c = c+1;
    }
}

void set_angle_servo(uint16_t wrap, int angle){
    if (angle>180){
        angle = 180;
    }
    if (angle<0){
        angle = 0;
    }
    int Percent_Conv = wrap*(10*angle/180)/100;

    printf("%d \n\r", Percent_Conv);
    pwm_set_gpio_level(LEDPin, Percent_Conv); // set the duty cycle to 12%
}

static inline void put_pixel(PIO pio, uint sm, uint32_t pixel_grb) {
    pio_sm_put_blocking(pio, sm, pixel_grb << 8u);
}

static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b) {
    return
            ((uint32_t) (r) << 8) |
            ((uint32_t) (g) << 16) |
            (uint32_t) (b);
}

static inline uint32_t urgbw_u32(uint8_t r, uint8_t g, uint8_t b, uint8_t w) {
    return
            ((uint32_t) (r) << 8) |
            ((uint32_t) (g) << 16) |
            ((uint32_t) (w) << 24) |
            (uint32_t) (b);
}



int main() {
    //set_sys_clock_48();
    stdio_init_all();
    printf("WS2812 Smoke Test, using pin %d\n", WS2812_PIN);

    // todo get free sm
    PIO pio;
    uint sm;
    uint offset;

    // This will find a free pio and state machine for our program and load it for us
    // We use pio_claim_free_sm_and_add_program_for_gpio_range (for_gpio_range variant)
    // so we will get a PIO instance suitable for addressing gpios >= 32 if needed and supported by the hardware
    bool success = pio_claim_free_sm_and_add_program_for_gpio_range(&ws2812_program, &pio, &sm, &offset, WS2812_PIN, 1, true);
    hard_assert(success);
    make_rainbow();
    ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, IS_RGBW);



    //PWM - 50Hz 150,000,000/60/50,000
    #define LEDPin 14 // the built in LED on the Pico
    gpio_set_function(LEDPin, GPIO_FUNC_PWM); // Set the LED Pin to be PWM
    uint slice_num = pwm_gpio_to_slice_num(LEDPin); // Get PWM slice number
    float div = 60; // must be between 1-255 
    pwm_set_clkdiv(slice_num, div); // divider
    uint16_t wrap = 50000; // when to rollover, must be less than 65535
    pwm_set_wrap(slice_num, wrap);
    pwm_set_enabled(slice_num, true); // turn on the PWM
  
    pwm_set_gpio_level(LEDPin, wrap*12/100); // set the duty cycle to 12%

    int t = 0;
    while (1) {
        int i = 0;
        for(i=0;i<LIST_LEN;i++){
            int second_one = i+120;
            if (second_one>360){
                second_one = second_one - 360;
            }
            int third_one = i+240; 
            if (third_one>360){
                third_one = third_one - 360;
            }
            put_pixel(pio, sm, urgb_u32(Rainbow[i].g, Rainbow[i].r, Rainbow[i].b)); // assuming you've made arrays of colors to send
            put_pixel(pio, sm, urgb_u32(Rainbow[second_one].g, Rainbow[second_one].r, Rainbow[second_one].b)); // assuming you've made arrays of colors to send
            put_pixel(pio, sm, urgb_u32(Rainbow[third_one].g, Rainbow[third_one].r, Rainbow[third_one].b)); // assuming you've made arrays of colors to send
            //put_pixel(pio, sm, urgb_u32(Rainbow[i+270].g, Rainbow[i+270].r, Rainbow[i+270].b)); // assuming you've made arrays of colors to send
            set_angle_servo(wrap,i/2);
            sleep_ms(13); // wait at least the reset time

        }
        sleep_ms(100); // wait at least the reset time
    }

    // This will free resources and unload our program
    pio_remove_program_and_unclaim_sm(&ws2812_program, pio, sm, offset);
}

wsColor HSBtoRGB(float hue, float sat, float brightness) {
    float red = 0.0;
    float green = 0.0;
    float blue = 0.0;

    if (sat == 0.0) {
        red = brightness;
        green = brightness;
        blue = brightness;
    } else {
        if (hue == 360.0) {
            hue = 0;
        }

        int slice = hue / 60.0;
        float hue_frac = (hue / 60.0) - slice;

        float aa = brightness * (1.0 - sat);
        float bb = brightness * (1.0 - sat * hue_frac);
        float cc = brightness * (1.0 - sat * (1.0 - hue_frac));

        switch (slice) {
            case 0:
                red = brightness;
                green = cc;
                blue = aa;
                break;
            case 1:
                red = bb;
                green = brightness;
                blue = aa;
                break;
            case 2:
                red = aa;
                green = brightness;
                blue = cc;
                break;
            case 3:
                red = aa;
                green = bb;
                blue = brightness;
                break;
            case 4:
                red = cc;
                green = aa;
                blue = brightness;
                break;
            case 5:
                red = brightness;
                green = aa;
                blue = bb;
                break;
            default:
                red = 0.0;
                green = 0.0;
                blue = 0.0;
                break;
        }
    }

    unsigned char ired = red * 255.0;
    unsigned char igreen = green * 255.0;
    unsigned char iblue = blue * 255.0;

    wsColor c;
    c.r = ired;
    c.g = igreen;
    c.b = iblue;
    return c;
}