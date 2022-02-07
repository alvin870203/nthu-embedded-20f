#include "mbed.h"
#include "uLCD_4DGL.h"

uLCD_4DGL uLCD(D1, D0, D2); // serial tx, serial rx, reset pin;
PwmOut PWM1(D6);
Serial pc( USBTX, USBRX );
DigitalIn D7data(D7);

int main()
{
    // uLCD
    uLCD.printf("\n105033110\n");
    uLCD.filled_rectangle(50, 50, 100, 90, 0x00FF00);

    // PWM
    int i; // 10*dutyCycle
    float data;
    while(1){
        for (i=0; i<=10; i++){
            PWM1.period(0.001); // 1000Hz
            PWM1 = ((float)i/10);
            wait(0.1);
            data = D7data;
            pc.printf("%f\r\n", data);
        }
        for (i=9; i>=1; i--){
            PWM1.period(0.001); // 1000Hz
            PWM1 = ((float)i/10);
            wait(0.1);
            data = D7data;
            pc.printf("%f\r\n", data);
        }   
    }

    // D7

    

}