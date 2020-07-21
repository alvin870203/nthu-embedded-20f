#include "mbed.h"
#include "bbcar.h"

Ticker servo_ticker;
PwmOut pin8(D6), pin9(D9); // prevent weird non-stop servo at speed 0, don't use D8&D9 same time

BBCar car(pin8, pin9, servo_ticker); // servo0, servo1

int main() {
    // please contruct you own calibration table with each servo
    // right : servo0 : +speed
    double pwm_table0[] = {-150, -120, -90, -60, -30, 0, 30, 60, 90, 120, 150};
    double speed_table0[] = {-17.226, -16.827, -15.870, -12.202, -5.343, 0.000, 7.337, 13.398, 16.349, 17.226, 17.625};
    // left : sero1 : -speed
    double pwm_table1[] = {-150, -120, -90, -60, -30, 0, 30, 60, 90, 120, 150};
    double speed_table1[] = {-17.306, -16.907, -15.551, -11.484, -4.944, 0.000, 6.699, 12.521, 15.790, 17.066, 17.465};    
    // first and fourth argument : length of table
    car.setCalibTable(11, pwm_table0, speed_table0, 11, pwm_table1, speed_table1);

    while (1) {
        car.goStraightCalib(5);
        wait(5);
        car.stop();
        wait(5);
    }
}