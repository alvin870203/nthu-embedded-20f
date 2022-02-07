#include "mbed.h"
#include "MQTTNetwork.h"
#include "MQTTmbed.h"
#include "MQTTClient.h"


//////////////////////////////FXOS8700CQ/////////////////////////////
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
int m_addr = FXOS8700CQ_SLAVE_ADDR1;
int idR[32] = {0};
int indexR = 0;

////////////////////////////////////////////////////////////////////


// GLOBAL VARIABLES
WiFiInterface *wifi;
InterruptIn btn2(SW2);
InterruptIn btn3(SW3);
volatile int message_num = 0;
volatile int arrivedcount = 0;
volatile bool closed = false;

const char* topic = "Mbed";

Thread mqtt_thread(osPriorityHigh);
EventQueue mqtt_queue;


//////////////////////////////FXOS8700CQ/////////////////////////////
void FXOS8700CQ_readRegs(int addr, uint8_t * data, int len) {
   char tmp = addr;
   i2c.write(m_addr, &tmp, 1, true);
   i2c.read(m_addr, (char *)data, len);
}

void FXOS8700CQ_writeRegs(uint8_t * data, int len) {
   i2c.write(m_addr, (char *)data, len);
}

void record(MQTT::Client<MQTTNetwork, Countdown>* client) {


   message_num++;
   MQTT::Message message;
   char buff[100];


   float x=0.0, y=0.0, z=0.0;
   int16_t acc16;
   uint8_t res[6];
   FXOS8700CQ_readRegs(FXOS8700Q_OUT_X_MSB, res, 6);
   acc16 = (res[0] << 6) | (res[1] >> 2);
   if (acc16 > UINT14_MAX/2)
     acc16 -= UINT14_MAX;
   x = ((float)acc16) / 4096.0f;
   acc16 = (res[2] << 6) | (res[3] >> 2);
   if (acc16 > UINT14_MAX/2)
     acc16 -= UINT14_MAX;
   y = ((float)acc16) / 4096.0f;
   acc16 = (res[4] << 6) | (res[5] >> 2);
   if (acc16 > UINT14_MAX/2)
     acc16 -= UINT14_MAX;
   z = ((float)acc16) / 4096.0f;
   //printf("%f, %f, %f\n", x * 1000.0, y * 1000.0, z * 1000.0);


   sprintf(buff, "FXOS8700Q ACC: X=%f, Y=%f, Z=%f #%d", x * 1000.0, y * 1000.0, z * 1000.0, message_num);
   message.qos = MQTT::QOS0;
   message.retained = false;
   message.dup = false;
   message.payload = (void*) buff;
   message.payloadlen = strlen(buff) + 1;
   int rc = client->publish(topic, message);

   printf("rc:  %d\r\n", rc);
   printf("Puslish message: %s\r\n", buff);


}

void initFXOS8700Q(void) {
  uint8_t data[2];
  FXOS8700CQ_readRegs( FXOS8700Q_CTRL_REG1, &data[1], 1);
  data[1] |= 0x01;
  data[0] = FXOS8700Q_CTRL_REG1;
  FXOS8700CQ_writeRegs(data, 2);
}

/////////////////////////////////////////////////////////////////////


void messageArrived(MQTT::MessageData& md) {
      MQTT::Message &message = md.message;
      char msg[300];
      sprintf(msg, "Message arrived: QoS%d, retained %d, dup %d, packetID %d\r\n", message.qos, message.retained, message.dup, message.id);
      printf(msg);
      wait_ms(1000);
      char payload[300];
      sprintf(payload, "Payload %.*s\r\n", message.payloadlen, (char*)message.payload);
      printf(payload);
      ++arrivedcount;
}


//////////////////////////////FXOS8700CQ/////////////////////////////
void publish_message(MQTT::Client<MQTTNetwork, Countdown>* client) {

      idR[indexR++] = mqtt_queue.call_every(500, &record, client);
      indexR = indexR % 32;
      
}

void close_mqtt() {
      for (auto &i : idR)
      mqtt_queue.cancel(i);
      closed = true;
}
/////////////////////////////////////////////////////////////////////


int main() {


//////////////////////////////FXOS8700CQ/////////////////////////////
      initFXOS8700Q();
/////////////////////////////////////////////////////////////////////


      wifi = WiFiInterface::get_default_instance();
      if (!wifi) {
            printf("ERROR: No WiFiInterface found.\r\n");
            return -1;
      }


      printf("\nConnecting to %s...\r\n", MBED_CONF_APP_WIFI_SSID);
      int ret = wifi->connect(MBED_CONF_APP_WIFI_SSID, MBED_CONF_APP_WIFI_PASSWORD, NSAPI_SECURITY_WPA_WPA2);
      if (ret != 0) {
            printf("\nConnection error: %d\r\n", ret);
            return -1;
      }


      NetworkInterface* net = wifi;
      MQTTNetwork mqttNetwork(net);
      MQTT::Client<MQTTNetwork, Countdown> client(mqttNetwork);

      //TODO: revise host to your ip
      const char* host = "192.168.43.242";
      printf("Connecting to TCP network...\r\n");
      int rc = mqttNetwork.connect(host, 1883);
      if (rc != 0) {
            printf("Connection error.");
            return -1;
      }
      printf("Successfully connected!\r\n");

      MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
      data.MQTTVersion = 3;
      data.clientID.cstring = "Mbed";

      if ((rc = client.connect(data)) != 0){
            printf("Fail to connect MQTT\r\n");
      }
      if (client.subscribe(topic, MQTT::QOS0, messageArrived) != 0){
            printf("Fail to subscribe\r\n");
      }

      mqtt_thread.start(callback(&mqtt_queue, &EventQueue::dispatch_forever));
      btn2.rise(mqtt_queue.event(&publish_message, &client));
      btn3.rise(&close_mqtt);

      int num = 0;
      while (num != 5) {
            client.yield(100);
            ++num;
      }

      while (1) {
            if (closed) break;
            wait(0.5);
      }

      printf("Ready to close MQTT Network......\n");

      if ((rc = client.unsubscribe(topic)) != 0) {
            printf("Failed: rc from unsubscribe was %d\n", rc);
      }
      if ((rc = client.disconnect()) != 0) {
      printf("Failed: rc from disconnect was %d\n", rc);
      }

      mqttNetwork.disconnect();
      printf("Successfully closed!\n");

      return 0;
}