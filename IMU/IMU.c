#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "IMU.h"
#include "ssd1306.h"
#include "font.h"
#include "math.h"

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA 16
#define I2C_SCL 17

#define ADDR_IMU 0x68
#define ACCEL_SCALE_FACTOR 0.000061f

void draw_my_message(char x, char y, char *message);
void draw_a_letter(char x, char y, char letter);
void line_draw(float x_val_g, float y_val_g);
void read_imu(void);

void I2C_Send_Data_init(void){
    uint8_t buf[2]; 
    buf[1] = 0x00; //cmd to turn on the chip
    buf[0] = PWR_MGMT_1 ; //register 
    i2c_write_blocking(i2c_default, ADDR_IMU, buf, 2, false);
    buf[1] = 0x00; //to turn on the chip
    buf[0] = ACCEL_CONFIG;
    i2c_write_blocking(i2c_default, ADDR_IMU, buf, 2, false);
    buf[1] = 0b11000; //to turn on the chip
    buf[0] = GYRO_CONFIG;
    i2c_write_blocking(i2c_default, ADDR_IMU, buf, 2, false);

    uint8_t reg[1];
    reg[0]= WHO_AM_I;
    buf[0]= 0b00000000;
    printf("\n%i\n\5", buf[0]);
    i2c_write_blocking(i2c_default, ADDR_IMU, reg, 1, true);  // true to keep master control of bus
    i2c_read_blocking(i2c_default, ADDR_IMU, buf, 1, false);  // false - finished with bus
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
    ssd1306_setup();



    while (true)
     {
        ssd1306_clear();
        char orgin[1];
        sprintf(orgin, "*");
        draw_my_message(62,12, orgin); 
        read_imu();
        ssd1306_update();
        sleep_ms(1000);

    }

}

void read_imu(){
    uint8_t reg = ACCEL_XOUT_H;
    uint8_t buf[6];

    // Request 6 bytes starting from ACCEL_XOUT_H 
    i2c_write_blocking(i2c_default, ADDR_IMU, &reg, 1, true);
    i2c_read_blocking(i2c_default, ADDR_IMU, buf, 6, false);

    int16_t x_val = (buf[0] << 8) | buf[1];
    int16_t y_val = (buf[2] << 8) | buf[3];
    int16_t z_val = (buf[4] << 8) | buf[5];

    float x_g = x_val * ACCEL_SCALE_FACTOR;
    float y_g = y_val * ACCEL_SCALE_FACTOR;
    float z_g = z_val * ACCEL_SCALE_FACTOR;

    line_draw(x_g, y_g);
    
}

void line_draw(float x_val_g, float y_val_g){
    float x_scale = x_val_g*8;
    float y_scale = y_val_g*8;
    int i = 0;
    while (i<16){
        if (i<x_scale){
            ssd1306_drawPixel(64-i,16+8,1);
            }
        if (i<y_scale){
            ssd1306_drawPixel(64+6,16+i,1);
            }
        if (i<-1*x_scale){
            ssd1306_drawPixel(64+i,16,1);
            }
        if (i<-1*y_scale){
            ssd1306_drawPixel(64,16-i,1);
            }
        i++;
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