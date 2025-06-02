#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "turning.h"
#define TURN_TOL 0

void turn_please(int com){
    if (com>(50+TURN_TOL)| com<(50-TURN_TOL)){
        right_wheel_control(com);
        left_wheel_control(100-com); //too bright for testing
        printf("com = %d, so we're Turning \n", com); 
}
}

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


void left_wheel_control(int duty_cycle){
    uint16_t wrap = 50000;
    if(duty_cycle>100){
        duty_cycle = 100;
    }
    if (duty_cycle<-100){
        duty_cycle = -100;
    }
    if (duty_cycle>0){
        gpio_put(LEFT_PHASE, 0);
        pwm_set_gpio_level(LEFT_WHEEL, wrap*duty_cycle/100); 
    }
    if (duty_cycle<=0){
        gpio_put(LEFT_PHASE, 1);
        duty_cycle = -1*duty_cycle;
        pwm_set_gpio_level(LEFT_WHEEL, wrap*duty_cycle/100); 
    }
}

void right_wheel_control(int duty_cycle){
    uint16_t wrap = 50000;
        if(duty_cycle>100){
        duty_cycle = 100;
    }
    if (duty_cycle<-100){
        duty_cycle = -100;
    }
    if (duty_cycle>0){
        gpio_put(RIGHT_PHASE, 0);
        pwm_set_gpio_level(RIGHT_WHEEL, wrap*duty_cycle/100); 
    }
    if (duty_cycle<=0){
        gpio_put(RIGHT_PHASE, 1);
        duty_cycle = -1*duty_cycle;
        pwm_set_gpio_level(RIGHT_WHEEL, wrap*duty_cycle/100); 
    }
}