#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include <math.h>


// SPI Defines
// We are going to use SPI 0, and allocate it to the following GPIO pins
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define SPI_PORT spi0
#define PIN_MISO 16
#define PIN_CS_DAC 17
#define PIN_CS_RAM 15
#define PIN_SCK  18
#define PIN_MOSI 19

#define WAVE_LEN 1000
#define WRITEMODE 0b00000010
#define READMODE 0b00000011

void SendData_DAC(int, float);
float ReadData_RAM();
void SendData_RAM();

union FloatInt { //"Double Cast" 
    float f; //4 Bites, SPI Write Fx Needs Bites (So Right Shit by 24)
    uint32_t i; 
};


static volatile union FloatInt Sin_Waveform[WAVE_LEN]; // waveforms


void make_Sin_Waveform(){
    for (int i=0; i<WAVE_LEN; i++){
        Sin_Waveform[i].f = 1.65*sin(i*4*3.14/(WAVE_LEN))+1.65; //done via a graphing calculator/trial and error
    }
}

static inline void cs_select(uint cs_pin) {
    asm volatile("nop \n nop \n nop"); // FIXME
    gpio_put(cs_pin, 0);
    asm volatile("nop \n nop \n nop"); // FIXME
}

static inline void cs_deselect(uint cs_pin) {
    asm volatile("nop \n nop \n nop"); // FIXME
    gpio_put(cs_pin, 1);
    asm volatile("nop \n nop \n nop"); // FIXME
}

void SPI_SetUp(){
    spi_init(SPI_PORT, WAVE_LEN*56*2); //myabe becasue 8*7 is 
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_CS_DAC,   GPIO_FUNC_SIO);
    //gpio_set_function(PIN_CS_RAM,   GPIO_FUNC_SIO);
    gpio_set_function(PIN_SCK,  GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    
    // Chip select is active-low, so we'll initialise it to a driven-high state
    gpio_set_dir(PIN_CS_DAC, GPIO_OUT);
    gpio_put(PIN_CS_DAC, 1);

    gpio_init(PIN_CS_RAM);
    gpio_set_dir(PIN_CS_RAM, GPIO_OUT);
    gpio_put(PIN_CS_RAM, 1);   
    // For more examples of SPI use see https://github.com/raspberrypi/pico-examples/tree/master/spi

}

void spi_ram_init(){
    uint8_t data_i[2];
    data_i[0] = 0b00000010; //to write // this one first 
    data_i[1] = 0b01000000 ;//to set op mode
    cs_select(PIN_CS_RAM);
    spi_write_blocking(SPI_PORT, data_i, 2); // where data is a uint8_t array with length len
    cs_deselect(PIN_CS_RAM);
}


int main()
{
    stdio_init_all();
    SPI_SetUp();   
    spi_ram_init();
    while (!stdio_usb_connected()) { //waiting for the screen to open the port 
        sleep_ms(100);
    }
    union FloatInt float_test ;
    float_test.f = 1.1; 
    uint8_t float_input[4]; 
    float_input[0] = (float_test.i>>24)&0xFF;
    float_input[1] = float_test.i & (0xFF <<16);
    float_input[2] = float_test.i & (0xFF <<8);
    float_input[3] = float_test.i & 0xFF ;
    printf("In Existance: float_input is: %b\n\r", float_test);
    
    uint8_t data_test[7] = {0,0,0,0,0,0,0};
    data_test[0] = WRITEMODE;//read mode 
    data_test[1] = 0b0;//adress of 0
    data_test[2] = 0b1;//address is still 1
    data_test[3] = float_input[0];
    data_test[4] = float_input[1];
    data_test[5] = float_input[2];
    data_test[6] = float_input[3]; 

    uint8_t data_returned[7] = {0,0,0,0,0,0,0};
    data_returned[0] = READMODE;//read mode 
    data_returned[1] = 0b0;//adress of 0
    data_returned[2] = 0b1;//address is still 1


    printf("Before Transfer: \n\r%b\n\r", data_test);
    printf("%b\n\r", data_returned);
    // printf("Before Transfer:\n\r%b   %b   %b   %b   %b   %b   %b \n\r", data_test[0],data_test[1],data_test[2],data_test[3],data_test[4],data_test[5],data_test[6]);
    // printf("%b   %b   %b   %b   %b   %b   %b   \n\r", data_returned[0],data_returned[1],data_returned[2],data_returned[3],data_returned[4],data_returned[5],data_returned[6]);
    cs_select(PIN_CS_RAM);
    spi_write_read_blocking(SPI_PORT, data_test, data_returned,14);
    cs_deselect(PIN_CS_RAM);
    printf("1st Transfer: \n\r%b\n\r", data_test);
    printf("%b\n\r", data_returned);
    // printf("After 1st Transfer:\n\r%b   %b   %b   %b   %b   %b   %b \n\r", data_test[0],data_test[1],data_test[2],data_test[3],data_test[4],data_test[5],data_test[6]);
    // printf("%b   %b   %b   %b   %b   %b   %b   \n\r", data_returned[0],data_returned[1],data_returned[2],data_returned[3],data_returned[4],data_returned[5],data_returned[6]);
    
    cs_select(PIN_CS_RAM);
    spi_write_read_blocking(SPI_PORT, data_test, data_returned,14);
    cs_deselect(PIN_CS_RAM);
    printf("2nd Transfer: \n\r%b\n\r", data_test);
    printf("%b\n\r", data_returned);
    // printf("After 2nd Transfer:\n\r%b   %b   %b   %b   %b   %b   %b \n\r", data_test[0],data_test[1],data_test[2],data_test[3],data_test[4],data_test[5],data_test[6]);
    // printf("%b   %b   %b   %b   %b   %b   %b   \n\r", data_returned[0],data_returned[1],data_returned[2],data_returned[3],data_returned[4],data_returned[5],data_returned[6]);
    
    //make_Sin_Waveform();
    //send entire waveform to external RAM 
    while (true) {
        //static int c = 0;
        //SendData_RAM();//    int read = 0b00000011 // to read mode 
        //float Return = ReadData_RAM();//get one point from external RAM that we want 
        //SendData_DAC(0,Return);//send waveform point to DAC 
        //sleep_ms(1000/WAVE_LEN); //wait
        //c++;
        //if( c == WAVE_LEN-1){//reset count
            //c = 0;
        }
    }


void SendData_DAC(int chan, float volt){
    uint8_t data[2];
    data[0] = 0;
    data[0] = data[0] | (chan << 7); //channel is the first bit 
    data[0] = data[0] | (0b111<<4); // Buffered, Gain, Shutdown 
    uint16_t volt_converted = 1024*volt/3.3;
    printf("Voltage We're Shooting For: %f \r\n", volt);
    //printf("Calculated Voltage Conversion: %u \r\n", volt_converted);
    int volt_data[10] = {0,0,0,0,0,0,0,0,0,0};  // Array to store binary digits
    int i = 0;
    int in = volt_converted;
    if (in > 1023){
        in = 1023; //10 bit number check
    }
    while(in > 0) {
        volt_data[9-i] = in % 2;  // Store remainder
        in /= 2;;  // Divide by 2 (this was a chat gpt find. i'm hoping that's ok)
        //printf("Volt Data = %d \n\r", volt_data[9-i]);
        i++;
    }
    printf("Volt Data = %d", volt_data);
    data[0] = data[0] | (volt_data[0]<<3);
    data[0] = data[0] | (volt_data[1]<<2);
    data[0] = data[0] | (volt_data[2]<<1);
    data[0] = data[0] | (volt_data[3]<<0); 

    data[1] = 0; 
    data[1] = data[1] | (volt_data[4]<<7); 
    data[1] = data[1] | (volt_data[5]<<6); 
    data[1] = data[1] | (volt_data[6]<<5); 
    data[1] = data[1] | (volt_data[7]<<4); 
    data[1] = data[1] | (volt_data[8]<<3); 
    data[1] = data[1] | (volt_data[9]<<2); 
    printf("%b %b \r\n", data[0], data[1]);
    cs_select(PIN_CS_DAC);
    spi_write_blocking(SPI_PORT, data, 2); // where data is a uint8_t array with length len
    cs_deselect(PIN_CS_DAC);
}

// void SendData_RAM(){
//     int write = 0b00000010 //to write // this one first 
//     spi_write_read_blocking();

// }

// float ReadData_RAM(){
//     int read = 0b00000011 //to read 
//     spi_write_read_blocking();
//     return data_returned;
// }