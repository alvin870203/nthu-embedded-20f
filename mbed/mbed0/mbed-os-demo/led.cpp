#include "mbed.h"

void Led(DigitalOut& ledpin)
{
   for(int i=0; i<2; ++i) { //blink for 10 times
       ledpin = !ledpin; // toggle led
       wait(0.2f);
   }
}