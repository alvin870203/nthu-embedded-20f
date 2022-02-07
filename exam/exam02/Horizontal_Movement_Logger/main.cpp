#include <math.h>
#include "mbed.h"
#include "fsl_port.h"
#include "fsl_gpio.h"
#define UINT14_MAX        16383
// FXOS8700CQ I2C address
#define FXOS8700CQ_SLAVE_ADDR0 (0x1E<<1) // with pins SA0=0, SA1=0
#define FXOS8700CQ_SLAVE_ADDR1 (0x1D<<1) // with pins SA0=1, SA1=0
#define FXOS8700CQ_SLAVE_ADDR2 (0x1C<<1) // with pins SA0=0, SA1=1
#define FXOS8700CQ_SLAVE_ADDR3 (0x1F<<1) // with pins SA0=1, SA1=1
// FXOS8700CQ internal register addresses
#define FXOS8700Q_STATUS 0x00
#define FXOS8700Q_OUT_X_MSB 0x01
#define FXOS8700Q_OUT_Y_MSB 0x03
#define FXOS8700Q_OUT_Z_MSB 0x05
#define FXOS8700Q_M_OUT_X_MSB 0x33
#define FXOS8700Q_M_OUT_Y_MSB 0x35
#define FXOS8700Q_M_OUT_Z_MSB 0x37
#define FXOS8700Q_WHOAMI 0x0D
#define FXOS8700Q_XYZ_DATA_CFG 0x0E
#define FXOS8700Q_CTRL_REG1 0x2A
#define FXOS8700Q_M_CTRL_REG1 0x5B
#define FXOS8700Q_M_CTRL_REG2 0x5C
#define FXOS8700Q_WHOAMI_VAL 0xC7

I2C i2c( PTD9,PTD8);
Serial pc(USBTX, USBRX);
int m_addr = FXOS8700CQ_SLAVE_ADDR1;

// EventQueue in a Thread
DigitalOut led(LED1);
InterruptIn sw2(SW2);
EventQueue queue(32 * EVENTS_EVENT_SIZE);
Thread thread1;

void FXOS8700CQ_readRegs(int addr, uint8_t * data, int len);
void FXOS8700CQ_writeRegs(uint8_t * data, int len);

// log data
void logger(float t[3]) {
    int i;
    int sample = 10; // sample time, log for 10 sec
    int n = 100; // number of the sample, 100 samples

    float x[n]; // log for x acceleration
    float y[n]; // log for y acceleration
    float z[n]; // log for z acceleration
    float x_begin; // x acceleration of initial stationary position
    int move[n]; // move over 5 cm
    float disp[n]; // displacement in same direction
    for (i = 0, x_begin = t[0] ; i < n; i++) {
        led = !led; // blink led as indicator
        x[i] = t[0];
        y[i] = t[1];
        z[i] = t[2];

        if ((x[i] > 0.1) || (x[i] < -0.1)) {
            disp[i] = 9.8*100*x[i]*0.1*0.1/2;
        } else {
            disp[i] = 0; // in case for samll error in x acceleration
        }
        
        if (i) {
            if ((x[i]*x[i-1]) > 0) {
                disp[i] += disp[i-1]; // same direction, add up
            } else {
                disp[i] = disp[i]; // oppossite direction, re-calculate
            }
        } else {
            disp[i] = 0;
        }
        
        if ((disp[i] >= 5) || (disp[i] <= -5)) {
            move[i] = 1; // over +/- 5 cm
        } else {
            move[i] = 0; // not
        }

        wait(float(sample)/n); // sampling interval 0.1 sec
    }
    led = 1; // turn off led when sampling is over
    
    // trasfer data to pc
    for (i = 0; i < n; i++) {
        pc.printf("%1.4f\r\n", x[i]);
    }
    for (i = 0; i < n; i++) {
        pc.printf("%1.4f\r\n", y[i]);
    }
    for (i = 0; i < n; i++) {
        pc.printf("%1.4f\r\n", z[i]);
    }
    for (i = 0; i < n; i++) {
        pc.printf("%d\r\n", move[i]);
    }
}

int main() {

    pc.baud(115200);

    uint8_t who_am_i, data[2], res[6];
    int16_t acc16;
    float t[3];
    
    led = 1; // set led off
    // EventQueue in a Thread
    thread1.start(callback(&queue, &EventQueue::dispatch_forever));
    sw2.rise(queue.event(logger, t));

    // Enable the FXOS8700Q
    FXOS8700CQ_readRegs( FXOS8700Q_CTRL_REG1, &data[1], 1);
    data[1] |= 0x01;
    data[0] = FXOS8700Q_CTRL_REG1;
    FXOS8700CQ_writeRegs(data, 2);

    // Get the slave address
    FXOS8700CQ_readRegs(FXOS8700Q_WHOAMI, &who_am_i, 1);

    while (true) {

        FXOS8700CQ_readRegs(FXOS8700Q_OUT_X_MSB, res, 6);

        acc16 = (res[0] << 6) | (res[1] >> 2);
        if (acc16 > UINT14_MAX/2)
            acc16 -= UINT14_MAX;
        t[0] = ((float)acc16) / 4096.0f;

        acc16 = (res[2] << 6) | (res[3] >> 2);
        if (acc16 > UINT14_MAX/2)
            acc16 -= UINT14_MAX;
        t[1] = ((float)acc16) / 4096.0f;

        acc16 = (res[4] << 6) | (res[5] >> 2);
        if (acc16 > UINT14_MAX/2)
            acc16 -= UINT14_MAX;
        t[2] = ((float)acc16) / 4096.0f;
    }
}

void FXOS8700CQ_readRegs(int addr, uint8_t * data, int len) {
    char t = addr;
    i2c.write(m_addr, &t, 1, true);
    i2c.read(m_addr, (char *)data, len);
}

void FXOS8700CQ_writeRegs(uint8_t * data, int len) {
    i2c.write(m_addr, (char *)data, len);
}