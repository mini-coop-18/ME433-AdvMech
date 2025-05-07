#include <stdio.h>
#include "pico/stdlib.h"
#include <math.h>  

#define CIR_LEN 400
#define CIR_RAD 800
#define CIR_OFF 0

static volatile float X_Circle_Waveform[2*CIR_LEN]; // waveforms
static volatile float Y_Circle_Waveform[2*CIR_LEN]; 
static volatile int x_control;
static volatile int y_control;

void make_circle(void){
        for(int i = 0; i<CIR_LEN;i++){
          float X_Val = (float)i*(2*CIR_RAD)/CIR_LEN;
          X_Circle_Waveform[i] = X_Val;
          float inside = CIR_RAD*CIR_RAD-(X_Val-CIR_RAD)*(X_Val-CIR_RAD);
          float Y_Val = sqrt(inside)-CIR_OFF;
          Y_Circle_Waveform[i] = Y_Val;
        //   printf("%i -- %f \t %f\n\r", i, X_Val, Y_Val);
        }
        for(int i = 0;i<CIR_LEN;i++){
          float X_Val = (CIR_LEN-(float)i)*(2*CIR_RAD)/CIR_LEN;
          X_Circle_Waveform[i+CIR_LEN] = X_Val;
          float inside = CIR_RAD*CIR_RAD-(X_Val-CIR_RAD)*(X_Val-CIR_RAD);
          float Y_Val = -1*sqrt(inside)-CIR_OFF;
          Y_Circle_Waveform[i+CIR_LEN] = Y_Val;
            printf("%i -- %f \t %f\n\r", i, X_Val, Y_Val);
        }
      }

int main()
{
    stdio_init_all();
    while (!stdio_usb_connected()) { //waiting for the screen to open the port 
        sleep_ms(100);
        }
    make_circle();

    while (true) {
        for(int i = 1;i<CIR_LEN*2;i++){
            // printf("i = %d -- %f     ", i, X_Circle_Waveform[i]-X_Circle_Waveform[i-1]);
            x_control = round(X_Circle_Waveform[i]-X_Circle_Waveform[i-1]);
            printf("%d\t", x_control);
            y_control = round(Y_Circle_Waveform[i]-Y_Circle_Waveform[i-1]);
            // printf("%f     ", Y_Circle_Waveform[i]-Y_Circle_Waveform[i-1]);
            printf("%d\n\r", y_control);
            sleep_ms(10);
    }
}
}
