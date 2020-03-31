import serial
import matplotlib.pyplot as plt
from drawnow import *
import atexit

P0 = []
P1 = []
x = []
plt.ion()
cnt=0

serialArduino = serial.Serial('/dev/ttyUSB0', 9600)

def plotValues():
    plt.title('Serial value from Arduino')
    plt.grid(True)
    plt.ylabel('Pressure')
    plt.plot(P0, 'rx-', label='P0')
    plt.plot(P1, 'bx-', label='P1')
    plt.legend(loc='upper right')

def doAtExit():
    serialArduino.close()
    print("Close serial")
    print("serialArduino.isOpen() = " + str(serialArduino.isOpen()))

atexit.register(doAtExit)

print("serialArduino.isOpen() = " + str(serialArduino.isOpen()))

#pre-load dummy data
for i in range(0,26):
    P0.append(0)
    P1.append(0)
while True:
    while (serialArduino.inWaiting()==0):
        pass
    print("readline()")
    valueRead = serialArduino.readline()
    print(valueRead)
    #valueInInt = int(valueRead)
   # x = valueRead.split()
    # print(valueRead[1])   
    Pressure0 = valueRead.decode().split(",")[0]
    print(Pressure0)
    Pressure1 = valueRead.decode().split(",")[1]
    print(Pressure1)
    #check if valid value can be casted
    try:
       
        valueInInt = int(Pressure0)
        valueInInt1 = int(Pressure1)
        print(valueInInt)
        if valueInInt <= 1024:
            
                P0.append(valueInInt)
                P0.pop(0)
                P1.append(valueInInt1)
                P1.pop(0)
                drawnow(plotValues)
            
        else:
            print("Invalid! too large")
    except ValueError:
        print("Invalid! cannot cast")
