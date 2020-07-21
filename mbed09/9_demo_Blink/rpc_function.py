import serial
import time
serdev = '/dev/ttyACM0'
s = serial.Serial(serdev)

s.write(bytes("\r", 'UTF-8'))
line=s.readline() # Read an echo string from K66F terminated with '\n' (pc.putc())
print(line)
line=s.readline() # Read an echo string from K66F terminated with '\n' (RPC reply)
print(line)
time.sleep(1)

s.write(bytes("/BlinkBOTH/run 5\r", 'UTF-8'))
line=s.readline() # Read an echo string from K66F terminated with '\n' (pc.putc())
print(line)
line=s.readline() # Read an echo string from K66F terminated with '\n' (RPC reply)
print(line)
time.sleep(1)

s.write(bytes("/BlinkRED/run 5\r", 'UTF-8'))
line=s.readline() # Read an echo string from K66F terminated with '\n' (pc.putc())
print(line)
line=s.readline() # Read an echo string from K66F terminated with '\n' (RPC reply)
print(line)
time.sleep(1)

s.write(bytes("/BlinkBLUE/run 5\r", 'UTF-8'))
line=s.readline() # Read an echo string from K66F terminated with '\n' (pc.putc())
print(line)
line=s.readline() # Read an echo string from K66F terminated with '\n' (RPC reply)
print(line)
time.sleep(1)

s.close()