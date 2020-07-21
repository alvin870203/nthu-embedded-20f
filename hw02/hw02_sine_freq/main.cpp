#include "mbed.h"

Serial pc( USBTX, USBRX);
AnalogIn Ain(A0);
DigitalIn  Switch(SW3);
DigitalOut redLED(LED1);
DigitalOut greenLED(LED2);
BusOut display(D6, D7, D9, D10, D11, D5, D4, D8); // 7_seg display
AnalogOut Aout(DAC0_OUT);

const int sample = 1000; // sampling freq (sampling rate) (>4(2)*freq) (for freq<100Hz)
float ADCdata[sample]={0}; // index
int i=0;

int freq=0; // signal frequency(round to int)
int zeroCross=0; // numbers of zero crossing
const float offset = 0.2988; // ADC readed input for offset 1V (zero) //doesn't affect freq much
int firstI=-1; // index of first zero crossing
int lastI=-1; // index of last zero crossing
int halfCycle=0; // number of half cycle (=zeroCross-1)

char table[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F}; // 0~9
int freqSSD=0; // =freq
int freqDigit[4] = {-1, -1, -1, -1}; // digits of freq, LSD~MSD, none is -1

float j=0;

int main(){
    // --------------------------------------------------------------- //
    // -- Sample the sine wave by AnalogIn Pin A0. ------------------- //
    // -- Calculate the frequency of the sine wave recieved from A0. - //
    // -- Send the sine wave data from A0 to PC. --------------------- //
    // --------------------------------------------------------------- //
    //sample analog input
    for (i=0; i<sample; i++){ // smaple for 1 sec
        ADCdata[i] = Ain;
        wait(1.f/sample); // wait(timeInterval)
    }
    // zero crossing detection
    zeroCross = 0;
    firstI = -1; // reset firstI
    lastI = -1; // reset lastI
    for (i=0; i<sample; i++){
        if ((ADCdata[i]>offset && ADCdata[i+1]<=offset)
            || (ADCdata[i]<offset && ADCdata[i+1]>=offset))
        {
            if (firstI==-1) firstI=i;
            lastI = i;
            zeroCross++;
        }
    }
    halfCycle = zeroCross - 1; // =(number of cycle)*2
    // freq = numOfCycle / (timeInterval*(lastI-fistI))
    //freq = (halfCycle/2.f) / ((1.f/sample)*(lastI-firstI+7)); // without correction factor
    freq = (halfCycle/2.f) / ((1.f/sample)*(lastI-firstI+20)); // +20 is correction factor
    if (freq<0) freq=0; //avoid negative caused by correction factor
    // send sine wave data to PC
    for (i=0; i<sample; i++){
        pc.printf("%1.3f\r\n", ADCdata[i]); // need time, so can't be in sample loop
    }
    // --------------------------------------------------------------- //


    // --------------------------------------------------------------- //
    // -- If button SW3 is pressed, turn on red led, ----------------- //
    // -- display freq using 7-seg display. -------------------------- //
    // -- Otherwise, turn off 7-seg display, turn on green led. ------ //
    // -- Generate sine wave with same freq with DAC output. --------- //
    // --------------------------------------------------------------- //
    // get digit of freq form LSB~MSB
    freqSSD = freq;
    if (freqSSD==0){
        freqDigit[0] = 0;
    }else{
        for (i=0; freqSSD>0; i++){
            freqDigit[i] = freqSSD % 10;
            freqSSD = freqSSD / 10;
        }
    }
    // button, led, display
    while(1){
        for (i=3; i>=0; i--){
            if(Switch == 0){ // 0 is press button
                greenLED = 1; // 1 is turn off
                redLED = 0; // 0 is turn on
                if (freqDigit[i] != -1){
                    display = (i==0) ? 
                        (table[freqDigit[i]]+0x80) : table[freqDigit[i]];
                    wait(0.5);
                }
            }else{ // 1 is not press button
                redLED = 1;
                greenLED = 0;
                display = 0x00;
                // Generate sine with same freq to DAC output
                if (freq>0){
                    for (j=0; j<2; j+=0.05){
                        Aout = 0.5 + 0.5*sin(j*3.14159);
                        wait_us(1000000/freq/40); // !problem: period is larger than expected somehow
                    }
                }else{
                    Aout = 0.5;
                }
                break;
            }   
        }

    }
    // --------------------------------------------------------------- //
}