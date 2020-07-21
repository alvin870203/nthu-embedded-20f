#include "mbed.h"
#include "mbed_rpc.h"

RpcDigitalOut myled_red(LED1, "myled_red");
RpcDigitalOut myled_blue(LED3, "myled_blue");
Serial pc(USBTX, USBRX);
void BlinkRED(Arguments *in, Reply *out);
void BlinkBLUE(Arguments *in, Reply *out);
void BlinkBOTH(Arguments *in, Reply *out);
RPCFunction rpcBlinkRED(&BlinkRED, "BlinkRED");
RPCFunction rpcBlinkBLUE(&BlinkBLUE, "BlinkBLUE");
RPCFunction rpcBlinkBOTH(&BlinkBOTH, "BlinkBOTH");
double blinkTime;

int main() {

    char buffer0[200], outbuf0[256];
    char strings0[20];
    int n = sprintf(strings0, "/myled_red/write 1");
    strcpy(buffer0, strings0);
    RPC::call(buffer0, outbuf0);
    n = sprintf(strings0, "/myled_blue/write 1");
    strcpy(buffer0, strings0);
    RPC::call(buffer0, outbuf0);

    //The mbed RPC classes are now wrapped to create an RPC enabled version - see RpcClasses.h so don't add to base class

    // receive commands, and send back the responses
    char buf[256], outbuf[256];
    while(1) {
        memset(buf, 0, 256);
        for (int i = 0; ; i++) {
            char recv = pc.getc();
            if (recv == '\r') {
                pc.printf("\r\n");
                break;
            }

            buf[i] = pc.putc(recv);
        }

        //Call the static call method on the RPC class
        RPC::call(buf, outbuf);
        pc.printf("%s\r\n", outbuf);
    }
}

void BlinkRED(Arguments *in, Reply *out) {
    // Have code here to call another RPC function to wake up specific led or close it.
    bool success = true;
    blinkTime = in->getArg<double>();
    char buffer[200], outbuf[256];
    char strings[20];
    int iTime = blinkTime;
    for (int i = 0; i < iTime; i++) {
        int n = sprintf(strings, "/myled_red/write 0");
        strcpy(buffer, strings);
        RPC::call(buffer, outbuf);
        wait(0.5);
        n = sprintf(strings, "/myled_red/write 1");
        strcpy(buffer, strings);
        RPC::call(buffer, outbuf);
        wait(0.5);
    }
    if (success) {
        pc.printf("Succeed to blink BLUE LED %d times", iTime);
    } else {
        pc.printf("Failed to execute LED control.");
    }
}

void BlinkBLUE(Arguments *in, Reply *out) {
    // Have code here to call another RPC function to wake up specific led or close it.
    bool success = true;
    blinkTime = in->getArg<double>();
    char buffer[200], outbuf[256];
    char strings[20];
    int iTime = blinkTime;
    for (int i = 0; i < iTime; i++) {
        int n = sprintf(strings, "/myled_blue/write 0");
        strcpy(buffer, strings);
        RPC::call(buffer, outbuf);
        wait(0.5);
        n = sprintf(strings, "/myled_blue/write 1");
        strcpy(buffer, strings);
        RPC::call(buffer, outbuf);
        wait(0.5);
    }
    if (success) {
        pc.printf("Succeed to blink BLUE LED %d times", iTime);
    } else {
        pc.printf("Failed to execute LED control.");
    }
}

void BlinkBOTH(Arguments *in, Reply *out) {
    // Have code here to call another RPC function to wake up specific led or close it.
    bool success = true;
    blinkTime = in->getArg<double>();
    char buffer[200], outbuf[256];
    char strings[20];
    int iTime = blinkTime;
    for (int i = 0; i < iTime; i++) {
        int n = sprintf(strings, "/myled_red/write 0");
        strcpy(buffer, strings);
        RPC::call(buffer, outbuf);
        wait(0.5);
        n = sprintf(strings, "/myled_red/write 1");
        strcpy(buffer, strings);
        RPC::call(buffer, outbuf);
        wait(0.5);
        n = sprintf(strings, "/myled_blue/write 0");
        strcpy(buffer, strings);
        RPC::call(buffer, outbuf);
        wait(0.5);
        n = sprintf(strings, "/myled_blue/write 1");
        strcpy(buffer, strings);
        RPC::call(buffer, outbuf);
        wait(0.5);
    }
    if (success) {
        pc.printf("Succeed to blink BOTH RED & BLUE LED %d times", iTime);
    } else {
        pc.printf("Failed to execute LED control.");
    }
}