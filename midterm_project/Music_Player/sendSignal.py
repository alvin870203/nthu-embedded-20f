import numpy as np
import serial
import sys
import time

serdev = '/dev/ttyACM0'
s = serial.Serial(serdev)

waitTime = 0.1

signalLength = 84

# little star
songA = [
0.261, 0.261, 0.392, 0.392, 0.440, 0.440, 0.392,
0.349, 0.349, 0.330, 0.330, 0.294, 0.294, 0.261,
0.392, 0.392, 0.349, 0.349, 0.330, 0.330, 0.294,
0.392, 0.392, 0.349, 0.349, 0.330, 0.330, 0.294,
0.261, 0.261, 0.392, 0.392, 0.440, 0.440, 0.392,
0.349, 0.349, 0.330, 0.330, 0.294, 0.294, 0.261,
0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.2,
0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.2,
0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.2,
0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.2,
0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.2,
0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.2
]

# Happy Birthday
songB = [
0.262, 0.262, 0.294, 0.262, 0.349, 0.330, 0.262,
0.262, 0.294, 0.262, 0.392, 0.349, 0.262, 0.262,
0.523, 0.440, 0.349, 0.330, 0.294, 0.466, 0.466,
0.440, 0.349, 0.392, 0.349, 0.262, 0.262, 0.294,
0.262, 0.349, 0.330, 0.262, 0.262, 0.294, 0.262,
0.392, 0.349, 0.262, 0.262, 0.523, 0.440, 0.349,
0.1, 0.1, 0.2, 0.2, 0.2, 0.3, 0.1,
0.1, 0.2, 0.2, 0.2, 0.3, 0.1, 0.1,
0.2, 0.2, 0.2, 0.2, 0.4, 0.1, 0.1,
0.2, 0.2, 0.2, 0.3, 0.1, 0.1, 0.2,
0.2, 0.2, 0.3, 0.1, 0.1, 0.2, 0.2,
0.2, 0.3, 0.1, 0.1, 0.2, 0.2, 0.2
]

#
songC = [
0.262, 0.294, 0.330, 0.330, 0.294, 0.262, 0.262,
0.262, 0.294, 0.294, 0.330, 0.294, 0.262, 0.330,
0.349, 0.392, 0.392, 0.349, 0.330, 0.262, 0.262,
0.262, 0.262, 0.294, 0.294, 0.330, 0.330, 0.349,
0.349, 0.330, 0.294, 0.262, 0.262, 0.294, 0.330,
0.330, 0.294, 0.262, 0.262, 0.262, 0.294, 0.294,
0.1, 0.1, 0.2, 0.1, 0.1, 0.2, 0.1,
0.1, 0.1, 0.1, 0.1, 0.1, 0.2, 0.1,
0.1, 0.2, 0.1, 0.1, 0.2, 0.3, 0.3,
0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1,
0.1, 0.1, 0.1, 0.3, 0.1, 0.1, 0.2,
0.1, 0.1, 0.2, 0.1, 0.1, 0.1, 0.1
]

while True:
    try:
        choice = s.readline().decode()
        if 'SongA' in choice:
            # generate the waveform table
            signalTable = np.array(songA)
            # output formatter
            formatter = lambda x: "%.3f" % x
            # send the waveform table to K66F
            print("Sending SongA signal ...")
            print("It may take about %d seconds ..." % (int(signalLength * waitTime)))
            for data in signalTable:
                s.write(bytes(formatter(data), 'UTF-8'))
                time.sleep(waitTime)
            print("SongA Signal sended")
        elif 'SongB' in choice:
            # generate the waveform table
            signalTable = np.array(songB)
            # output formatter
            formatter = lambda x: "%.3f" % x
            # send the waveform table to K66F
            print("Sending SongB signal ...")
            print("It may take about %d seconds ..." % (int(signalLength * waitTime)))
            for data in signalTable:
                s.write(bytes(formatter(data), 'UTF-8'))
                time.sleep(waitTime)
            print("SongB Signal sended")
        elif 'SongC' in choice:
            # generate the waveform table
            signalTable = np.array(songC)
            # output formatter
            formatter = lambda x: "%.3f" % x
            # send the waveform table to K66F
            print("Sending SongC signal ...")
            print("It may take about %d seconds ..." % (int(signalLength * waitTime)))
            for data in signalTable:
                s.write(bytes(formatter(data), 'UTF-8'))
                time.sleep(waitTime)
            print("SongA Signal sended")        
    except KeyboardInterrupt:
        print("Exiting...")
        s.close()
        sys.exit()
        