#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include <math.h>

// SPI Defines
// We are going to use SPI 0, and allocate it to the following GPIO pins
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define WAVE_LEN 1000

#define SPI_PORT spi0
#define PIN_MISO 16
#define PIN_CS   17
#define PIN_SCK  18
#define PIN_MOSI 19

static volatile float Tri_Waveform[WAVE_LEN]; // waveforms
static volatile float Sin_Waveform[WAVE_LEN]; // waveforms

void SendData(int, float);
int* decimal_to_binary(int);


void make_Tri_Waveform() {
    for (int i=0; i<WAVE_LEN-1; i++){
        if(i<WAVE_LEN/2){
            Tri_Waveform[i] =(3.3/(WAVE_LEN/2))*i;
        }
        if(i>=WAVE_LEN/2){
            Tri_Waveform[i] = -(3.3/(WAVE_LEN/2))*i+6.6;
        }
    }
}
  

void make_Sin_Waveform(){
    for (int i=0; i<WAVE_LEN-1; i++){
        Sin_Waveform[i] = 1.65*sin(i*4*3.14/(WAVE_LEN))+1.65; //done via a graphing calculator/trial and error
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

int main()
{
    stdio_init_all();

    // SPI initialisation. This example will use SPI at 1MHz.
    spi_init(SPI_PORT, WAVE_LEN*2*16);
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_CS,   GPIO_FUNC_SIO);
    gpio_set_function(PIN_SCK,  GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    
    // Chip select is active-low, so we'll initialise it to a driven-high state
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_put(PIN_CS, 1);
    // For more examples of SPI use see https://github.com/raspberrypi/pico-examples/tree/master/spi

    make_Tri_Waveform();
    make_Sin_Waveform();

    while (true) {
        static int c = 0;
        SendData(1,Sin_Waveform[c]);
        SendData(0,Tri_Waveform[c]);
        sleep_ms(1000/WAVE_LEN); //wait
        c++;
        if( c == WAVE_LEN-1){//reset count
            c = 0;
        }
    }
}

void SendData(int chan, float volt){
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
    cs_select(PIN_CS);
    spi_write_blocking(SPI_PORT, data, 2); // where data is a uint8_t array with length len
    cs_deselect(PIN_CS);
}

