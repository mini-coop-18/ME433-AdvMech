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

#define ADDR 0x68

void read_imu(void);

void I2C_Send_Data_init(void){
    uint8_t buf[2]; 
    buf[1] = 0x00; //cmd to turn on the chip
    buf[0] = PWR_MGMT_1 ; //register 
    i2c_write_blocking(i2c_default, ADDR, buf, 2, false);
    buf[1] = 0b0; //to turn on the chip
    buf[0] = ACCEL_CONFIG;
    i2c_write_blocking(i2c_default, ADDR, buf, 2, false);
    buf[1] = 0b11000; //to turn on the chip
    buf[0] = ACCEL_CONFIG;
    i2c_write_blocking(i2c_default, ADDR, buf, 2, false);

    uint8_t reg[1];
    reg[0]= WHO_AM_I;
    buf[0]= 0b00000000;
    printf("\n%i\n\5", buf[0]);
    i2c_write_blocking(i2c_default, ADDR, reg, 1, true);  // true to keep master control of bus
    i2c_read_blocking(i2c_default, ADDR, buf, 1, false);  // false - finished with bus
    printf("\n%i\n\r",buf[0]); 
    sleep_ms(100);
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

    while (true)
     {
        read_imu();
        sleep_ms(10);

    }

}

void read_imu(){
    uint8_t buf[1]; 
    uint8_t x_reg_high[1];
    uint8_t x_reg_low[1];
    //X Data 
    buf[0] = ACCEL_XOUT_H;
    i2c_write_blocking(i2c_default, ADDR, buf, 1, true);  // true to keep master control of bus
    i2c_read_blocking(i2c_default, ADDR, x_reg_high, 1, false);  // false - finished with bus
    buf[0] = ACCEL_XOUT_L;
    i2c_write_blocking(i2c_default, ADDR, buf, 1, true);  // true to keep master control of bus
    i2c_read_blocking(i2c_default, ADDR, x_reg_low, 1, false);  // false - finished with bus
    int16_t x_val; 
    x_val = (x_reg_high[0])<<8; 
    x_val = x_val|(x_reg_low[0]);
    float x_val_g = (float)x_val*0.000061;
    printf("X = %i \t", x_val);
    printf("X = %f\n\r", x_val_g);

    uint8_t y_reg_high[1];
    uint8_t y_reg_low[1];
    //Y Data 
    buf[0] = ACCEL_YOUT_H;
    i2c_write_blocking(i2c_default, ADDR, buf, 1, true);  // true to keep master control of bus
    i2c_read_blocking(i2c_default, ADDR, y_reg_high, 1, false);  // false - finished with bus
    buf[0] = ACCEL_YOUT_L;
    i2c_write_blocking(i2c_default, ADDR, buf, 1, true);  // true to keep master control of bus
    i2c_read_blocking(i2c_default, ADDR, y_reg_low, 1, false);  // false - finished with bus
    int16_t y_val; 
    y_val = (y_reg_high[0]<<8); 
    y_val = y_val|(y_reg_low[0]);
    float y_val_g = 0.000061*(float)y_val;
    printf("Y = %i \t", y_val);
    printf("Y= %f\n\r", y_val_g);

    uint8_t z_reg_high[1];
    uint8_t z_reg_low[1];
    //Z Data 
    buf[0] = ACCEL_ZOUT_H;
    i2c_write_blocking(i2c_default, ADDR, buf, 1, true);  // true to keep master control of bus
    i2c_read_blocking(i2c_default, ADDR, z_reg_high, 1, false);  // false - finished with bus
    buf[0] = ACCEL_ZOUT_L;
    i2c_write_blocking(i2c_default, ADDR, buf, 1, true);  // true to keep master control of bus
    i2c_read_blocking(i2c_default, ADDR, z_reg_low, 1, false);  // false - finished with bus
    int16_t z_val; 
    // printf("%b\n", z_reg_high[0]);
    // printf("%b\n", z_reg_low[0]); 
    z_val = z_reg_high[0]<<8; 
    z_val = z_val|(z_reg_low[0]);
    // printf("%b\n", z_val);
    float z_val_g = 0.000061*(float)z_val;
    printf("Z = %i \t", z_val);
    printf("Z= %f\n\r\n\n", z_val_g);
}

void accel_line(Ax, Ay){
    int orgin_x = 
    int orgion_y = 
}