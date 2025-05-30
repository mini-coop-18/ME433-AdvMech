#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"


#define LEFT_BLINKER 15
#define LEFT_WHEEL 17
#define LEFT_PHASE 18

#define RIGHT_WHEEL 19 
#define RIGHT_BLINKER 20
#define RIGHT_PHASE 21


void init_left_side(void){
    gpio_init(LEFT_PHASE); 
    gpio_set_dir(LEFT_PHASE, 1);
    gpio_put(LEFT_PHASE, 0);

    gpio_init(LEFT_WHEEL);  
    gpio_set_function(LEFT_WHEEL, GPIO_FUNC_PWM); 
    uint left_slice_num = pwm_gpio_to_slice_num(LEFT_WHEEL); // Get PWM slice number
    float div = 60; 
    uint16_t wrap = 50000; // when to rollover, must be less than 65535
    pwm_set_gpio_level(LEFT_WHEEL, wrap*50/100); // set the duty cycle to 50%
    pwm_set_clkdiv(left_slice_num, div); // divider
    pwm_set_wrap(left_slice_num, wrap);
    pwm_set_enabled(left_slice_num, true); // turn on the PWM

    gpio_init(LEFT_BLINKER);  
    gpio_set_dir(LEFT_BLINKER, 1); 
    gpio_put(LEFT_BLINKER, 1);

}

void init_right_side(void){
    gpio_init(RIGHT_PHASE); 
    gpio_set_dir(RIGHT_PHASE, 1);
    gpio_put(RIGHT_PHASE, 0);

    gpio_init(RIGHT_WHEEL);  
    gpio_set_function(RIGHT_WHEEL, GPIO_FUNC_PWM); 
    uint right_slice_num = pwm_gpio_to_slice_num(RIGHT_WHEEL); // Get PWM slice number
    float div = 60; 
    uint16_t wrap = 50000; // when to rollover, must be less than 65535
    pwm_set_gpio_level(RIGHT_WHEEL, wrap*50/100); // set the duty cycle to 50%
    pwm_set_clkdiv(right_slice_num, div); // divider
    pwm_set_wrap(right_slice_num, wrap);
    pwm_set_enabled(right_slice_num, true); // turn on the PWM

    gpio_init(RIGHT_BLINKER);  
    gpio_set_dir(RIGHT_BLINKER, 1); 
    gpio_put(RIGHT_BLINKER, 1);

}

int main()
{
    stdio_init_all();
    init_left_side(); 
    init_right_side();
    int left_dc = 50; 
    int right_dc = 50; 
    uint16_t wrap = 50000;
    while (true) {
        printf("Left or Right Sir?");
        char dir_msg[2];
        scanf("%c", dir_msg);
        if (dir_msg == "r"){
            right_dc = right_dc + 10; 
            pwm_set_gpio_level(RIGHT_WHEEL, wrap*right_dc/100); // set the duty cycle to 50%
            pwm_set_gpio_level(LEFT_WHEEL, 0); 
        }
    }
}
