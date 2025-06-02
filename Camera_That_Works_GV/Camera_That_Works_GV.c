#include <stdio.h>
#include "pico/stdlib.h"
#include "cam.h"

int main()
{
    stdio_init_all();

    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }
    //printf("Hello, camera!\n");

    init_camera_pins();
 
    while (true) {
        // uncomment these and printImage() when testing with python 
        // printf("cmd: \n\r");
        char m[10];
        scanf("%s",m);

        setSaveImage(1);
        // printf("Saving Image is now 1");
        while(getSaveImage()==1){}
        // printf("Saving Image is not one");
        convertImage();
        // printf("Converted Image");
        int com = findLine(IMAGESIZEY/2); // calculate the position of the center of the ine
        // printf("FindLine Function");
        setPixel(IMAGESIZEY/2,com,0,255,0); // draw the center so you can see it in python
        // printf("About to Print Image");
        printImage();
        // printf("%d\r\n",com); // comment this when testing with python
    }
}

