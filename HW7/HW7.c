#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "ssd1306.h"
#include "font.h"

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA 16  
#define I2C_SCL 17

#define POT_PIN 26
#define POT_PIN_A 0


void draw_my_message(char x, char y, char *message);
void draw_a_letter(char x, char y, char letter);


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


int main()
{
    stdio_init_all();

    int rc = pico_led_init();
    hard_assert(rc == PICO_OK);
    pico_set_led(true);

    adc_init(); // init the adc module
    adc_gpio_init(POT_PIN); // set ADC0 pin to be adc input instead of GPIO
    adc_select_input(POT_PIN_A); // select to read from ADC0

    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400*1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // while (!stdio_usb_connected()) { //waiting for the screen to open the port 
    //     sleep_ms(100);
    // }
    ssd1306_setup();

    while (true) {
        ssd1306_clear();
        absolute_time_t t1 = get_absolute_time();
        uint64_t t_1 = to_us_since_boot(t1);
        for(int k=0; k<1000; k++){
        uint16_t result = adc_read(); //reading the adc 
        float result_v = (float)3.3*result/4095; //convert to volts
        char my_text[50]; 
        sprintf(my_text, "Pot. Read: %.4f V", result_v);
        //printf("Printing %s \n\r", my_text);
        char x = 10;
        char y = 10;
        draw_my_message(x,y,my_text);
        // sleep_ms(100);
        }

        absolute_time_t t2 = get_absolute_time();
        uint64_t t_2 = to_us_since_boot(t2); 
        char time_delta[50];
        sprintf(time_delta, "Up Rt: %.1f", (float)(t_2-t_1)/6.67);
        draw_my_message(10+40, 10+12, time_delta);
        ssd1306_update();

    }
}

void draw_my_message(char x, char y, char* message){
    int i = 0 ;
    //ssd1306_clear();
	while(message[i] != 0){ // while it’s not 0 
        draw_a_letter(x+5*i, y, message[i]);
        i++;
    }
}

void draw_a_letter(char x, char y, char letter){
    int i, j; 
    for (i = 0; i<5; i++){
        char col = ASCII[letter-32][i];
        for (j = 0; j<8; j++){
            char on = (col>>j)&0b1;
            //printf("letter -- %s. Bit %d x %d =  %b \n\r", letter, i, j, on);
            ssd1306_drawPixel(x+i,y+j,on);
        }
    }
}