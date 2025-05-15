#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "IMU.h"

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA 16
#define I2C_SCL 17

#define ADDR 0x98

void I2C_Send_Data_init(void){
    uint8_t buf[2]; 
    buf[1] = 0x00; //to turn on the chip
    buf[0] = PWR_MGMT_1 ;
    i2c_write_blocking(i2c_default, ADDR, buf, 2, false);
    buf[1] = 0x00; //to turn on the chip
    buf[0] = ACCEL_CONFIG;
    i2c_write_blocking(i2c_default, ADDR, buf, 2, false);
    buf[1] = 0b11000; //to turn on the chip
    buf[0] = ACCEL_CONFIG;
    i2c_write_blocking(i2c_default, ADDR, buf, 2, false);

}


int main()
{
    stdio_init_all();

    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400*1000);
    
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    // For more examples of I2C use see https://github.com/raspberrypi/pico-examples/tree/master/i2c

    I2C_Send_Data_init();

    while (true) {
        uint8_t reg[1];
        reg[0]= WHO_AM_I;
        uint8_t buf[1] ;
        buf[0]= 0b00000000;
        printf("\n%i\n\5", buf);
        i2c_write_blocking(i2c_default, ADDR, reg, 1, true);  // true to keep master control of bus
        i2c_read_blocking(i2c_default, ADDR, buf, 1, false);  // false - finished with bus
        printf("\n%i\n\r",buf); 
        printf("%b\n\r",buf);
        sleep_ms(500);
    }

}
