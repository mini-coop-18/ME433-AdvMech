#include <stdio.h>
#include "pico/stdlib.h"


int main()
{
    stdio_init_all();

    while (true) {
        volatile float f1, f2;
        volatile float f_add, f_sub, f_mult, f_div;

        printf("Enter two floats to use:");
        scanf("%f %f", &f1, &f2);
        printf("\nTwo Floats We're Mathing: %f and %f \n\r", f1,f2);
        absolute_time_t t1 = get_absolute_time();
        uint64_t t_1 = to_us_since_boot(t1);
        for (int i = 0; i<1000;i++) 
        {
        f_add = f1+f2;
        //f_sub = f1-f2;
        //f_mult = f1*f2;
        //f_div = f1/f2;
        }
        absolute_time_t t2 = get_absolute_time();
        uint64_t t_2 = to_us_since_boot(t2); 
        printf("Time to Add =  %f\n\r", (float)(t_2-t_1)*1000);
        
        t1 = get_absolute_time();
        t_1 = to_us_since_boot(t1);
        for (int i = 0; i<1000;i++) {
        //f_add = f1+f2;
        f_sub = f1-f2;
        //f_mult = f1*f2;
        //f_div = f1/f2;
        //printf("\nResults: \n%f+%f=%f \n%f-%f=%f \n%f*%f=%f \n%f/%f=%f\n", f1,f2,f_add, f1,f2,f_sub, f1,f2,f_mult, f1,f2,f_div);
        }
        t2 = get_absolute_time();
        t_2 = to_us_since_boot(t2); 
        printf("Time to Subtract = %f\n\r", (float)(t_2-t_1)*1000);

        t1 = get_absolute_time();
        t_1 = to_us_since_boot(t1);
        for (int i = 0; i<1000;i++) 
        {
        //f_add = f1+f2;
        //f_sub = f1-f2;
        f_mult = f1*f2;
        //f_div = f1/f2;
        //printf("\nResults: \n%f+%f=%f \n%f-%f=%f \n%f*%f=%f \n%f/%f=%f\n", f1,f2,f_add, f1,f2,f_sub, f1,f2,f_mult, f1,f2,f_div);
        }
        t2 = get_absolute_time();
        t_2 = to_us_since_boot(t2); 
        printf("Time to Multiply = %f\n\r", (float)(t_2-t_1)*1000);

        t1 = get_absolute_time();
        t_1 = to_us_since_boot(t1);
        for (int i = 0; i<1000;i++) 
        {
        //f_add = f1+f2;
        //f_sub = f1-f2;
        //f_mult = f1*f2;
        f_div = f1/f2;
        //printf("\nResults: \n%f+%f=%f \n%f-%f=%f \n%f*%f=%f \n%f/%f=%f\n", f1,f2,f_add, f1,f2,f_sub, f1,f2,f_mult, f1,f2,f_div);
        }
        t2 = get_absolute_time();
        t_2 = to_us_since_boot(t2); 
        
        printf("%llu \n\r", t_1);
        printf("%llu\n\r",t_2);
        printf("Time to Divide = %f\n\r", (float)(t_2-t_1)*1000); //Convert from ms to clock cylces

        printf("\nResults: \n%f+%f=%f \n%f-%f=%f \n%f*%f=%f \n%f/%f=%f\n", f1,f2,f_add, f1,f2,f_sub, f1,f2,f_mult, f1,f2,f_div);

    }
}
