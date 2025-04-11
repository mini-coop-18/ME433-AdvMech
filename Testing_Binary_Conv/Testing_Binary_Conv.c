#include <stdio.h>
#include "pico/stdlib.h"


int main()
{
    stdio_init_all();
    while (!stdio_usb_connected()) { //waiting for the screen to open the port 
        sleep_ms(100);
    }
    while(2){
    int in;
    printf("\nWhat would you like to convert? \r\n");
    scanf("%d", &in); 
    printf("Ok Converting %d \r\n", in);
    int binary[10] = {0,0,0,0,0,0,0,0,0,0};  // Array to store binary digits
    int i = 0;
    while(in > 0) {
        binary[9-i] = in % 2;  // Store remainder
        in /= 2;;  // Divide by 2 (this was a chat gpt find. i'm hoping that's ok)
        printf("At i = %d: Binary = %d \n\r", i, binary[9-i]);
        i++;
    }
    for (int m = 0; m<10; m++){
        printf("%d", binary[m]);
    }
    }
}
