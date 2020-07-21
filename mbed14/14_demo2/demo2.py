import pyb
import sensor, image, time, os

uart = pyb.UART(3,9600,timeout_char=1000)
uart.init(9600,bits=8,parity = None, stop=1, timeout_char=1000)
tmp = ""

sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.skip_frames(time = 2000)
sensor.set_auto_gain(False) # must turn this off to prevent image washout...

def QR_Code_Decoding():
    while(1):
        img = sensor.snapshot()
        img.lens_corr(1.8) # strength of 1.8 is good for the 2.8mm lens.
        for code in img.find_qrcodes():
           return code.payload()

while(1):
   a = uart.readline()
   if a is not None:
      tmp += a.decode()
      print(a.decode())

   if tmp == "QR_Code_Decoding":
      print("Decode QR Code")
      tmp =""
      result = QR_Code_Decoding()
      print(result)
      uart.write(("%s\r\n" % result).encode())
