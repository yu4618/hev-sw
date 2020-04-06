#!/usr/bin/python3
# Karol's dumb serial dumper

import serial
import time

ser = serial.Serial(
    port='/dev/ttyUSB0',
    baudrate=9600,
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_ONE,
    bytesize=serial.EIGHTBITS,
    timeout=1
    )
for i in range(20) : 
    print(ser.read(15).hex())
