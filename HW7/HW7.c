#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "ssd1306.h"
#include "font.h"

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA 12  
#define I2C_SCL 13

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

    int rc = pico_led_init();
    hard_assert(rc == PICO_OK);
    pico_set_led(true);


    stdio_init_all();
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
    char my_text[50]; 
    sprintf(my_text, "Hi Grace!");
    printf("Printing %s \n\r", my_text);
    char x = 10;
    char y = 10;
    draw_my_message(x,y,my_text);

    while (true) {
    }
}

void draw_my_message(char x, char y, char* message){
    int i = 0 ;
    ssd1306_clear();
	while(message[i] != 0){ // while it’s not 0 
        draw_a_letter(x+5*i, y, message[i]);
        i++;
    }
    ssd1306_update();
}

void draw_a_letter(char x, char y, char letter){
    int i, j; 
    for (i = 0; i<5; i++){
        char col = ASCII[letter-32][i];
        for (j = 0; j<8; j++){
            char on = (col>>j)&0b1;
            printf("letter -- %s. Bit %d x %d =  %b \n\r", letter, i, j, on);
            ssd1306_drawPixel(x+i,y+j,on);
        }
    }
}