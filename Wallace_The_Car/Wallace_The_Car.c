#include <stdio.h>
#include "pico/stdlib.h"

#include <stdio.h>
#include "pico/stdlib.h"
#include "cam.h"
#include "turning.h"


int main()
{
    stdio_init_all();
    init_camera_pins();
    init_left_side(); 
    init_right_side();

    while (true) {
        setSaveImage(1);
        while(getSaveImage()==1){}
        convertImage();
        gpio_put(LEFT_BLINKER, 0);
        int com = findLine(IMAGESIZEY/2); // calculate the position of the center of the ine
        turn_please(com);
        gpio_put(LEFT_BLINKER, 1);
    }
}
