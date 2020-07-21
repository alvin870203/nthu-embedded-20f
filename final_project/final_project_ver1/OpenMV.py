import pyb
import sensor, image, time, os, math, tf

uart = pyb.UART(3,9600,timeout_char=1000)
uart.init(9600,bits=8,parity = None, stop=1, timeout_char=1000)
tmp = ""

sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.set_windowing((240, 240))
sensor.skip_frames(time = 2000)
sensor.set_auto_gain(False) # must turn this off to prevent image washout...
sensor.set_auto_whitebal(False)  # must turn this off to prevent image washout...
clock = time.clock()

labels = ['3', '4', '0', 'other']

def Data_Matrix_Decoding():
    while(True):
        clock.tick()
        img = sensor.snapshot()
        img.lens_corr(1.8) # strength of 1.8 is good for the 2.8mm lens.
        matrices = img.find_datamatrices()
        for matrix in matrices:
            return str(matrix.payload()) + ", " + str((180 * matrix.rotation()) / math.pi)

def Image_Classification_MNIST():
    sensor.skip_frames(time = 2000)
    img = sensor.snapshot()
    img.save("Mission1_Snapshot.jpg")
    for obj in tf.classify('/model_demo.tflite',img, min_scale=1.0, scale_mul=0.5, x_overlap=0.0, y_overlap=0.0):
        return str(labels[obj.output().index(max(obj.output()))]))

while(1):
   a = uart.readline()
   if a is not None:
      tmp += a.decode()
      print(a.decode())

   if tmp == "Data_Matrix_Decoding":
      print("Decode Data Matrix")
      tmp =""
      result = Data_Matrix_Decoding()
      print(result)
      uart.write(("(%s)\r\n" % result).encode())

   if tmp == "Image_Classification_MNIST":
      print("Classify MNIST Image")
      tmp =""
      result = Image_Classification_MNIST()
      print(result)
      uart.write(("(%s)\r\n" % result).encode())


