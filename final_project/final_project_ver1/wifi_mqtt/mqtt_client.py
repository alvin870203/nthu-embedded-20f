import paho.mqtt.client as paho
import time
import matplotlib.pyplot as plt
import numpy as np
import sys

data = []
data_new = []

# https://os.mbed.com/teams/mqtt/wiki/Using-MQTT#python-client

# MQTT broker hosted on local machine
mqttc = paho.Client()

# Settings for connection
host = "localhost"
topic= "bbcar"
port = 1883

# Callbacks
def on_connect(self, mosq, obj, rc):
    print("Connected rc: " + str(rc))

# Subscribe log data from MQTT srver, and stored as Log_Data.txt
def on_message(mosq, obj, msg):
    text = str(msg.payload.decode().rstrip('\x00'))
    print(text)
    data_new.append(text )
    if '---------- End ----------' in text:
        data.append(data_new.copy())
        print("Log data saved")
        filename = "Log_Data"+".txt"
        with open(filename, "w") as f:
            for lines in data:
                for line in lines:
                        f.write(line)
                        f.write("\n")
        print("Save file in", filename)

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

# Loop forever, receiving messages
mqttc.loop_forever()
