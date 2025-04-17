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

#define WAVE_LEN 10
#define WRITEMODE 0b00000010
#define READMODE 0b00000011
#define MEM_START 2000

union FloatInt { //"Double Cast" 
    float f; //4 Bites, SPI Write Fx Needs Bites (So Right Shit by 24)
    uint32_t i; 
};

void SendData_DAC(int, float);
float ReadData_RAM();
void SendData_RAM();
void RAM_Send(union FloatInt, int);
float RAM_Get(int);

static volatile float Sin_Waveform[WAVE_LEN]; // waveforms


void make_Sin_Waveform(){
    union FloatInt sin_wave;
    for (int i=0; i<WAVE_LEN; i++){
        Sin_Waveform[i] = 1.65*sin(i*4*3.14/(WAVE_LEN))+1.65; //done via a graphing calculator/trial and error
        sin_wave.f = Sin_Waveform[i];
        int i_adj = i+MEM_START;
        RAM_Send(sin_wave,i_adj);
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
    data_i[0] = 0b00000001; //to write // this one first 
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
    make_Sin_Waveform();
    printf("Waveform Loaded In RAM\n");

    for (int i = 0;i<WAVE_LEN;i++){
        printf("\n At %d - We're Hoping For: %f \r\n", i, Sin_Waveform[i]);
        sleep_ms(5);
        int i_adj = i+MEM_START;
        float voltage_rec = RAM_Get(i_adj);
        //SendData_DAC(0, voltage_rec);         
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



void RAM_Send(union FloatInt Val, int addr){
    uint8_t data_send[7] = {0,0,0,0,0,0,0};
    uint8_t data_get[7] = {0,0,0,0,0,0,0}; 
    int addr_val = addr;
    // int i = 0;
    //int adr_data[16] = {0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0};  // Array to store binary digits
    // while(addr > 0) {
    //     adr_data[15-i] = addr % 2;  // Store remainder
    //     addr /= 2;;  // Divide by 2 (this was a chat gpt find. i'm hoping that's ok)
    //     //printf("Volt Data = %d \n\r", volt_data[9-i]);
    //     i++;
    // }
    data_send[1] = (addr_val >> 8) & 0xFF;
    data_send[2] = addr_val & 0xFF;

    // data_send[1] = data_send[1] | (adr_data[0]<<7); 
    // data_send[1] = data_send[1] | (adr_data[1]<<6); 
    // data_send[1] = data_send[1] | (adr_data[2]<<5); 
    // data_send[1] = data_send[1] | (adr_data[3]<<4); 
    // data_send[1] = data_send[1] | (adr_data[4]<<3); 
    // data_send[1] = data_send[1] | (adr_data[5]<<2); 
    // data_send[1] = data_send[1] | (adr_data[6]<<1); 
    // data_send[1] = data_send[1] | (adr_data[7]<<0); 

    // data_send[2] = data_send[2] | (adr_data[8]<<7); 
    // data_send[2] = data_send[2] | (adr_data[9]<<6); 
    // data_send[2] = data_send[2] | (adr_data[10]<<5); 
    // data_send[2] = data_send[2] | (adr_data[11]<<4); 
    // data_send[2] = data_send[2] | (adr_data[12]<<3); 
    // data_send[2] = data_send[2] | (adr_data[13]<<2); 
    // data_send[2] = data_send[2] | (adr_data[14]<<1); 
    // data_send[2] = data_send[2] | (adr_data[15]<<0); 
    printf("Making the Address: %d = %b %b \n\r", addr_val, data_send[1], data_send[2]);

    data_send[0] = WRITEMODE;//read mode 
    data_send[3] = (Val.i>>24)&0xFF;
    data_send[4] = (Val.i>>16)&0xFF;
    data_send[5] = (Val.i>>8)&0xFF;
    data_send[6] = (Val.i>>0)&0xFF;

    cs_select(PIN_CS_RAM);
    spi_write_read_blocking(SPI_PORT, data_send, data_get,14);
    cs_deselect(PIN_CS_RAM);
    printf("Sent: %b   %b   %b   %b   %b   %b   %b \n\r", data_send[0],data_send[1],data_send[2],data_send[3],data_send[4],data_send[5],data_send[6]);
    // printf("Returned: %b   %b   %b   %b   %b   %b   %b   \n\r", data_get[0],data_get[1],data_get[2],data_get[3],data_get[4],data_get[5],data_get[6]);
    
}

float RAM_Get(int addr){
    uint8_t data_send[7] = {0,0,0,0,0,0,0};
    data_send[0] = READMODE;//read mode 
    int addr_val = addr;
    // int i = 0;
    char adr_data[16] = {0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0};  // Array to store binary digits
    // while(addr > 0) {
    //     adr_data[15-i] = addr % 2;  // Store remainder
    //     addr /= 2;;  // Divide by 2 (this was a chat gpt find. i'm hoping that's ok)
    //     //printf("Volt Data = %d \n\r", volt_data[9-i]);
    //     i++;
    // }

    data_send[1] = (addr_val >> 8) & 0xFF;
    data_send[2] = addr_val & 0xFF;

    // data_send[1] = data_send[1] | (adr_data[0]<<7); 
    // data_send[1] = data_send[1] | (adr_data[1]<<6); 
    // data_send[1] = data_send[1] | (adr_data[2]<<5); 
    // data_send[1] = data_send[1] | (adr_data[3]<<4); 
    // data_send[1] = data_send[1] | (adr_data[4]<<3); 
    // data_send[1] = data_send[1] | (adr_data[5]<<2); 
    // data_send[1] = data_send[1] | (adr_data[6]<<1); 
    // data_send[1] = data_send[1] | (adr_data[7]<<0); 

    // data_send[2] = data_send[2] | (adr_data[8]<<7); 
    // data_send[2] = data_send[2] | (adr_data[9]<<6); 
    // data_send[2] = data_send[2] | (adr_data[10]<<5); 
    // data_send[2] = data_send[2] | (adr_data[11]<<4); 
    // data_send[2] = data_send[2] | (adr_data[12]<<3); 
    // data_send[2] = data_send[2] | (adr_data[13]<<2); 
    // data_send[2] = data_send[2] | (adr_data[14]<<1); 
    // data_send[2] = data_send[2] | (adr_data[15]<<0); 
    printf("Making the Address: %d = %b %b \n\r", addr_val, data_send[1], data_send[2]);


    uint8_t data_returned[7] = {0,0,0,0,0,0,0};
    // printf("Before Transfer:\n\rSent: %b   %b   %b   %b   %b   %b   %b \n\r", data_send[0],data_send[1],data_send[2],data_send[3],data_send[4],data_send[5],data_send[6]);
    // printf("Returned: %b   %b   %b   %b   %b   %b   %b   \n\r", data_returned[0],data_returned[1],data_returned[2],data_returned[3],data_returned[4],data_returned[5],data_returned[6]);
    cs_select(PIN_CS_RAM);
    spi_write_read_blocking(SPI_PORT, data_send, data_returned,14);
    cs_deselect(PIN_CS_RAM);
    // printf("After Transfer:\n\r Sent: b   %b   %b   %b   %b   %b   %b \n\r", data_send[0],data_send[1],data_send[2],data_send[3],data_send[4],data_send[5],data_send[6]);
    printf("Returned: %b   %b   %b   %b   %b   %b   %b   \n\r", data_returned[0],data_returned[1],data_returned[2],data_returned[3],data_returned[4],data_returned[5],data_returned[6]);
    
    union FloatInt Volt_Rec;
    Volt_Rec.i = 0; 
    Volt_Rec.i = Volt_Rec.i | (data_returned[3]&0xFF)<<24;
    Volt_Rec.i = Volt_Rec.i | (data_returned[4]&0xFF)<<16;
    Volt_Rec.i = Volt_Rec.i | (data_returned[5]&0xFF)<<8;
    Volt_Rec.i = Volt_Rec.i | (data_returned[6]&0xFF);

    float Volt_Rec_f = Volt_Rec.f;
    printf("We're Getting Back: %f \n\r", Volt_Rec_f);
    return Volt_Rec_f;
}