#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA 12  
#define I2C_SCL 13

#define ADDR  0b0100000
#define DIR_REG 0b00000001


#define LED_DELAY_MS 1000


void I2C_Send_Data_init();
void I2C_Read_Data(uint8_t buf, uint8_t reg);
void I2C_LX_On();
void I2C_LX_Off();
int I2C_Button_Read();

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

    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400*1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    // For more examples of I2C use see https://github.com/raspberrypi/pico-examples/tree/master/i2c
    I2C_Send_Data_init();
    while (true) {
            while(I2C_Button_Read()>0){
                I2C_LX_On();
                sleep_ms(LED_DELAY_MS);
            }
            I2C_LX_Off();
            pico_set_led(true); 
            sleep_ms(LED_DELAY_MS);
            while(I2C_Button_Read()>0){
                I2C_LX_On();
                sleep_ms(LED_DELAY_MS);
            }
            I2C_LX_Off();
            pico_set_led(false);
            sleep_ms(LED_DELAY_MS);
    }
}

void I2C_Send_Data_init(void){
    uint8_t buf[2]; 
    buf[0] = 0x00; //IODIR 
    buf[1] = DIR_REG;
    i2c_write_blocking(i2c_default, ADDR, buf, 2, false);
}

void I2C_LX_On(){
    uint8_t buf[2]; 
    buf[0] = 0x09; //GPIO Register
    buf[1] = 0b10000000; //Turning the LX on 
    i2c_write_blocking(i2c_default, ADDR, buf, 2, false);
}

void I2C_LX_Off(){
    uint8_t buf[2]; 
    buf[0] = 0x09; //GPIO Register
    buf[1] = 0b00000000; //Turning the LX on 
    i2c_write_blocking(i2c_default, ADDR, buf, 2, false);
}

int I2C_Button_Read(){
    uint8_t buf[1]; 
    uint8_t reg[1];

    reg[0] = 0x09; //GPIO register

    i2c_write_blocking(i2c_default, ADDR, reg, 1, true);  // true to keep master control of bus
    i2c_read_blocking(i2c_default, ADDR, buf, 1, false);  // false - finished with bus

    int button_byte = (buf[0]<<7)&0xFF; 
    printf("%d \n\r", button_byte);
    //printf("%b \n\r\n\n", lower_byte);
    return button_byte; 
}