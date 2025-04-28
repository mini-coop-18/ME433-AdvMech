#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/pwm.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "ws2812.pio.h"


#define NUM_PIXELS 4;
#define LED_PIN 13; 
#define WS2812_PIN 13;

#define IS_RGBW false


// link three 8bit colors together
typedef struct {
    unsigned char r;
    unsigned char g;
    unsigned char b;
} wsColor; 

wsColor HSBtoRGB(float hue, float sat, float brightness);
void set_angle_servo(uint16_t wrap, int angle);
void put_pixel(uint8_t g, uint8_t r, uint8_t b);

static inline void put_pixel(PIO pio, uint sm, uint32_t pixel_grb) {
    pio_sm_put_blocking(pio, sm, pixel_grb << 8u);
}

int main()
{
    stdio_init_all();

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

    ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, IS_RGBW);


    PIO pio;
    uint sm;
    uint offset;

    while (true) {

        int i;
        for(i=0;i<NUM_PIXELS;i++){
            put_pixel(LED_PIN, sm, urgb_u32(10, 10, 10)); // assuming you've made arrays of colors to send
        }

        sleep_ms(1); // wait at least the reset time

        // int i = 0;
        // while(i<180){
        //     set_angle_servo(wrap,i);
        //     i = i+1;
        //     sleep_ms(10);
        // }
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


// adapted from https://forum.arduino.cc/index.php?topic=8498.0
// hue is a number from 0 to 360 that describes a color on the color wheel
// sat is the saturation level, from 0 to 1, where 1 is full color and 0 is gray
// brightness sets the maximum brightness, from 0 to 1
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