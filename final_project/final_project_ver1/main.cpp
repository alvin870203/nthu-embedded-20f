#include "mbed.h"
#include "arm_math.h"
#include "bbcar.h"
#include <math.h>
#include <stdlib.h>
#define STRAIGHT_SPEED 16
#define TURN_SPEED 100
#define TURN_WAIT 3.0

DigitalOut led1(LED1);
PwmOut pin9(D9), pin8(D5);
DigitalInOut pin10(D10);
Ticker servo_ticker;
Ticker encoder_ticker;
DigitalIn pin3(D3);
Serial xbee(D12, D11);

BBCar car(pin8, pin9, servo_ticker);

void reply_messange(char *xbee_reply, char *messange);
void check_addr(char *xbee_reply, char *messenger);

Thread thread1;
Thread thread2;
Serial uart(D1,D0); //tx,rx

int main()
{
    parallax_ping ping1(pin10);
    parallax_encoder encoder0(pin3, encoder_ticker);

    // right : servo0 : +speed
    double pwm_table0[] = {-150, -120, -90, -60, -30, 0, 30, 40, 70, 120, 150};
    double speed_table0[] = {-17.226, -16.827, -15.870, -12.202, -5.343, 0.000, 7.337, 7.4, 17, 17.226, 17.625};
    // left : sero1 : -speed
    double pwm_table1[] = {-150, -120, -90, -60, -30, 0, 30, 60, 90, 120, 150};
    double speed_table1[] = {-17.306, -16.907, -15.551, -11.484, -4.944, 0.000, 6.699, 12.521, 15.790, 17.066, 17.465};    
    // first and fourth argument : length of table
    car.setCalibTable(11, pwm_table0, speed_table0, 11, pwm_table1, speed_table1);

    // XBee setting
    char xbee_reply[4];
    xbee.baud(9600);
    xbee.printf("+++");
    xbee_reply[0] = xbee.getc();
    xbee_reply[1] = xbee.getc();
    if(xbee_reply[0] == 'O' && xbee_reply[1] == 'K'){
        xbee_reply[0] = '\0';
    xbee_reply[1] = '\0';
    }
    xbee.printf("ATRE\r\n");
    reply_messange(xbee_reply, "reset the XBee");

    xbee.printf("ATMY 0x427\r\n");
    reply_messange(xbee_reply, "setting MY : 0x427");

    xbee.printf("ATDL 0x327\r\n");
    reply_messange(xbee_reply, "setting DL : 0x327");

    xbee.printf("ATID 0x2\r\n");
    reply_messange(xbee_reply, "setting PAN ID : 0x2");

    xbee.printf("ATWR\r\n");
    reply_messange(xbee_reply, "write config");

    xbee.printf("ATMY\r\n");
    check_addr(xbee_reply, "MY");

    xbee.printf("ATDL\r\n");
    check_addr(xbee_reply, "DL");

    xbee.printf("ATCN\r\n");
    reply_messange(xbee_reply, "exit AT mode");
    xbee.getc();

    xbee.printf("---------- START ----------\n");
    // ************************************************************************
    // *************************** 1. Enter the map ***************************
    // ************************************************************************
    // 1-1. Go straight till car is < 30 cm in fornt of the data matrix 
    xbee.printf("\nEnter the Map\n -> GoStraight\n");
    car.goStraightCalib(STRAIGHT_SPEED);
    while (1) {
        if ((float)ping1 > 30) { // ??? Need timeout ???
            led1 = 1;
        } else {
            //led1 = 0;
            car.stop();
            break;
        }
        wait(0.01);
    }
    led1 = 0;
    wait(1);
    xbee.printf("\nEnter the Map\n -> GoStraight\n");

    // 1-2. Scan data matrix
    led1 = 1;
    // send serial port to OpenMV
    char s[21];
    sprintf(s,"Data_Matrix_Decoding");
    uart.puts(s);
    wait(0.5);
    // recieve from OpenMV by serial port: payload and angle
    char data_matrix_data[100] = "";
    int count = 0;
    while(1) {
        if(uart.readable()){
            char recv = uart.getc();
            data_matrix_data[count] = recv;
            count++;
            if (recv == ')')
                break;
        }
    }
    data_matrix_data[count] = '\0';
    xbee.printf(" -> data matrix%s\n", data_matrix_data);
    led1 = 0;
    wait(1);

    // 1-3. Turn left 90deg
    xbee.printf(" -> TurnLeft\n");
    car.turn(TURN_SPEED, 0.1); // ??? Need to reset factor after ???
    wait(TURN_WAIT);
    car.stop();
    led1 = 0;
    wait(1);

    // ***********************************************************************
    // **************************** 2. Mission 1 *****************************
    // ***********************************************************************
    // 2-1. Go straight till car is < 21cm in fornt of the wall
    xbee.printf("\nMission1\n -> GoStraight\n");
    car.goStraightCalib(STRAIGHT_SPEED);
    while (1) {
        if ((float)ping1 > 21) {
            led1 = 1;
        } else {
            //led1 = 0;
            car.stop();
            break;
        }
        wait(0.01);
    }
    xbee.printf(" -> %f cm in front of the wall\n", (float)ping1);
    led1 = 0;
    wait(1);

    // 2-2. Reverse parking to ID 2
    //      - Go left-back till car is > 50cm in front of the wall
    xbee.printf(" -> Reverse parking\n");
    car.turn(-TURN_SPEED, 0.1); // ??? Need to adjust parameters ???
    wait(TURN_WAIT);
    car.stop();
    wait(0.5);
    car.goStraight(-STRAIGHT_SPEED);
    wait(1);
    car.stop();
    led1 = 0;
    wait(1);

    // 2-3. Classify MINIST image & Snapshot
    // 2-3-1. Turn right a little bit to get MINIST imgage in the middele of camera
    led1 = 1;
    car.turn(TURN_SPEED, -0.5);//////////////////////////////////////////
    wait(0.5); // ??? adjust how little the turn shoul be ???////////////
    car.stop();
    // 2-3-2. Image classification
    // send serial port to OpenMV
    char s2[21];
    sprintf(s2,"Image_Classification_MNIST");
    uart.puts(s2);
    wait(0.5);
    // MINIST: recieve from OpenMV by serial port
    char MINIST_data[100] = "";
    int count2 = 0;
    while(1) {
        if(uart.readable()){
            char recv = uart.getc();
            MINIST_data[count2] = recv;
            count2++;
            if (recv == ')')
                break;
        }
    }
    MINIST_data[count-3] = ' ';
    MINIST_data[count] = '\0';
    xbee.printf(" -> MINIST%s\n", MINIST_data);
    xbee.printf(" -> Snapshot\n");
    led1 = 0;
    wait(1);

    // 2-5. Exit parking
    led1 = 1;
    xbee.printf(" -> Exit parking\n");
    car.turn(TURN_SPEED, -0.5);//////////////////////////////////////////
    wait(1.5);///////////////////////////////////////////////////////////
    car.stop();
    led1 = 0;
    wait(1);

    // 2-6. Exit Mission1
    led1 = 1;
    xbee.printf(" -> Exit Mission1\n");
    car.turn(TURN_SPEED, -0.1);//////////////////////////////////////////
    wait(TURN_WAIT + 1.0);///////////////////////////////////////////////
    car.stop();
    led1 = 0;
    wait(1);

    // ***********************************************************************
    // **************************** 3. Mission 2 *****************************
    // ***********************************************************************
    // 3-1. Go straight till car is < 30cm in fornt of the wall
    xbee.printf("\nMission2\n -> GoStraight\n");
    car.goStraightCalib(STRAIGHT_SPEED);
    while (1) {
        if ((float)ping1 > 30) { // ??? Need timeout ???
            led1 = 1;
        } else {
            //led1 = 0;
            car.stop();
            break;
        }
        wait(0.01);
    }
    led1 = 0;
    wait(1);

    // 3-2. Turn right 90deg to enter Mission2
    led1 = 1;
    xbee.printf(" -> TurnRight and enter Mission2\n");
    car.turn(TURN_SPEED, -0.1); // ??? Need to reset factor after ???/////////
    wait(TURN_WAIT);
    car.stop();
    led1 = 0;
    wait(1);

    // 3-3. Go straight to the ID 2
    led1 = 1;
    xbee.printf(" -> GoStraight to spot\n");
    car.goStraightCalib(STRAIGHT_SPEED);
    encoder0.reset();
    while (encoder0.get_cm() < 55) { /////////////////////////////////////////
        wait_ms(50);
    }
    car.stop();
    led1 = 0;
    wait(1);

    // 3-4. Turn right 90deg then left 90deg to scan object by PING
    led1 = 1;
    xbee.printf(" -> Scan object by PING: \n");
    car.turn(TURN_SPEED / 2.0, -0.1); /////////////////////////////////////
    wait(TURN_WAIT);
    car.stop();
    wait(0.5);
    car.turn(TURN_SPEED / 2.0, 0.1); //////////////////////////////////////
    wait(TURN_WAIT / 10.0);
    double depth1 = (float)ping1;
    wait(TURN_WAIT / 10.0);
    double depth2 = (float)ping1;
    wait(TURN_WAIT / 10.0);
    double depth3 = (float)ping1;
    wait(TURN_WAIT / 10.0);
    double depth4 = (float)ping1;
    wait(TURN_WAIT / 10.0);
    double depth5 = (float)ping1;
    wait(TURN_WAIT / 2.0);
    car.stop();
    // identify algorithm
    if (depth1 >= depth2 && depth4 <= depth5 && (depth3 <= depth2 || depth3 <= depth4)) {
        xbee.printf(" -> PING object type(Sharp)\n");
    } else if (depth1 == depth2 && depth2 == depth3 && depth3 == depth4 && depth4 == depth5) {
        xbee.printf(" -> PING object type(Flat)\n");
    } else if (depth1 > depth2 && depth2 > depth3 && depth3 > depth4 && depth4 > depth5) {
        xbee.printf(" -> PING object type(Tilted)\n");
    } else if (depth1 <= depth2 && depth4 >= depth5 && (depth3 >= depth2 || depth3 >= depth4)) {
        xbee.printf(" -> PING object type(Concave)\n");
    } else {
        xbee.printf(" -> PING object type(Unknown)\n");
    }
    led1 = 0;
    wait(1.0);

    // ***********************************************************************
    // ******************************* 4. End ********************************
    // ***********************************************************************
    // 3-1. Go straight till car is < 30cm in fornt of the wall
    xbee.printf("\n---------- End ----------\n");
    led1 = 1;
    car.goStraightCalib(STRAIGHT_SPEED);
    wait(5.0);
    led1 = 0;
    while (1) {
        wait(0.5);
        led1 = !led1;
    }

}

void reply_messange(char *xbee_reply, char *messange){
  xbee_reply[0] = xbee.getc();
  xbee_reply[1] = xbee.getc();
  xbee_reply[2] = xbee.getc();
  if(xbee_reply[1] == 'O' && xbee_reply[2] == 'K'){
    xbee_reply[0] = '\0';
    xbee_reply[1] = '\0';
    xbee_reply[2] = '\0';
  }
}

void check_addr(char *xbee_reply, char *messenger){
  xbee_reply[0] = xbee.getc();
  xbee_reply[1] = xbee.getc();
  xbee_reply[2] = xbee.getc();
  xbee_reply[3] = xbee.getc();
  xbee_reply[0] = '\0';
  xbee_reply[1] = '\0';
  xbee_reply[2] = '\0';
  xbee_reply[3] = '\0';
}