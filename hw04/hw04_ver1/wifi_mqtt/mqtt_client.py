import paho.mqtt.client as paho
import time
import matplotlib.pyplot as plt
import numpy as np

# https://os.mbed.com/teams/mqtt/wiki/Using-MQTT#python-client

# MQTT broker hosted on local machine
mqttc = paho.Client()

# Settings for connection
# TODO: revise host to your ip
host = "192.168.43.242"
topic = "Mbed"


x_log = []
y_log = []
z_log = []
sampling_interval = []
timestamp = []

# Callbacks
def on_connect(self, mosq, obj, rc):
      print("Connected rc: " + str(rc))

def on_message(mosq, obj, msg):
      #print("[Received] Topic: " + msg.topic + ", Message: " + str(msg.payload) + "\n");
      text = str(msg.payload.decode().rstrip('\x00'))
      print(text)
      float_list = [float(x) for x in text.split(', ')]
      x_log.append(float_list[0])
      y_log.append(float_list[1])
      z_log.append(float_list[2])
      sampling_interval.append(float_list[3])

def on_subscribe(mosq, obj, mid, granted_qos):
      print("Subscribed OK")

def on_unsubscribe(mosq, obj, mid, granted_qos):
      print("Unsubscribed OK")

# Set callbacks
mqttc.on_message = on_message
mqttc.on_connect = on_connect
mqttc.on_subscribe = on_subscribe
mqttc.on_unsubscribe = on_unsubscribe

# Connect and subscribe
print("Connecting to " + host + "/" + topic)
mqttc.connect(host, port=1883, keepalive=60)
mqttc.subscribe(topic, 0)

# Publish messages from Python
#num = 0
#while num != 5:
#      ret = mqttc.publish(topic, "Message from Python!\n", qos=0)
#      if (ret[0] != 0):
#            print("Publish failed")
#      mqttc.loop()
#      time.sleep(1.5)
#      num += 1

# Loop forever, receiving messages
time.sleep(6)
mqttc.loop() # get something out
mqttc.loop() # get something out

num = 0
while num != 200: # will get num messages
    mqttc.loop()
    if (num == 0):
        timestamp.append(sampling_interval[num])
    else:
        timestamp.append(timestamp[num - 1] + sampling_interval[num])
    if (timestamp[num] > 20):
        break
    time.sleep(0.6)
    num += 1

#print(timestamp)

# plot
fig, ax = plt.subplots()
ax.plot(timestamp, x_log, "-", color="green", label="x-acc")
ax.plot(timestamp, y_log, "-", color="red", label="y-acc")
ax.plot(timestamp, z_log, "-", color="blue", label="z-acc")
ax.legend(loc='lower left', frameon=True)
ax.set_xlabel('timestamp')
ax.set_ylabel('acc value')
plt.show()
