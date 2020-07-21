import matplotlib.pyplot as plt
import numpy as np
import serial
import time

sample = 10; # sample time, log for 10 sec
Ts = 0.1; # sampling interval
t = np.arange(0,sample,Ts) # time vector; create n samples between 0 and 10 sec.
n = len(t) # length of the signal, number of the sample, 100 samples

x_log = np.arange(0,sample,Ts) # x acc signal vector; create n samples
y_log = np.arange(0,sample,Ts) # y acc signal vector; create n samples
z_log = np.arange(0,sample,Ts) # z acc signal vector; create n samples
move = np.arange(0,sample,Ts) # movement event signal vector; create n samples

serdev = '/dev/ttyACM0'
s = serial.Serial(serdev, 115200)

for x in range(0, n):
    x_line = s.readline() # Read an echo string from K66F terminated with '\n'
    # print line
    x_log[x] = float(x_line)

for x in range(0, n):
    y_line = s.readline() # Read an echo string from K66F terminated with '\n'
    # print line
    y_log[x] = float(y_line)

for x in range(0, n):
    z_line = s.readline() # Read an echo string from K66F terminated with '\n'
    # print line
    z_log[x] = float(z_line)

for x in range(0, n):
    move_line = s.readline() # Read an echo string from K66F terminated with '\n'
    # print line
    move[x] = int(move_line)

fig, ax = plt.subplots(2, 1)
ax[0].plot(t,x_log, color="blue", label="x")
ax[0].plot(t,y_log, color="red", label="y")
ax[0].plot(t,z_log, color="green", label="z")
ax[0].legend(loc='lower left', frameon=True)
ax[0].set_xlabel('Time')
ax[0].set_ylabel('Acc Vector')
ax[1].stem(t, move, use_line_collection=True)
ax[1].set_xlabel('Time')
ax[1].set_ylabel('Movement')
plt.show()
s.close()