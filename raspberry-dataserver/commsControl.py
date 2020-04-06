#!/usr/bin/env python3

# Communication protocol between rasp and arduino based on HDLC format
# author Peter Svihra <peter.svihra@cern.ch>

import serial
from serial.tools import list_ports

import time

import commsFormat
from collections import deque
import logging
logging.basicConfig(level=logging.DEBUG, format='%(asctime)s - %(levelname)s - %(message)s')

import asyncio
import serial_asyncio

# communication class that governs talking between devices
class commsControl(asyncio.Protocol):
    def __init__(self, queueSize = 16):
        # takes care of all serial port initializations
        super().__init__()
        self.transport_ = None

        # queues are FIFO ring-buffers of the defined size
        self.alarms_   = deque(maxlen = queueSize)
        self.commands_ = deque(maxlen = queueSize)
        self.data_     = deque(maxlen = queueSize)
        
        # needed to find packet frames
        self.received_ = []
        self.foundStart_ = False
        self.timeLastTransmission_ = int(round(time.time() * 1000))
        
    # virtual function from asyncio.Protocol
    def connection_made(self, transport):
        self.transport_ = transport
        logging.info('port opened')
        self.transport_.serial.rts = False # no idea what this is, copy-pasted
     
    # virtual function from asyncio.Protocol, reads from serial port, "random" number of bytes
    def data_received(self, data):
        self.receiver(data)
     
    # virtual function from asyncio.Protocol
    def connection_lost(self, exc):
        logging.info('port closed')
        self.transport_.loop.stop()
     
    # virtual function from asyncio.Protocol
    def pause_writing(self):
        logging.info('pause writing')
        logging.debug(self.transport_.get_write_buffer_size())
    
    # virtual function from asyncio.Protocol
    def resume_writing(self):
        logging.debug(self.transport_.get_write_buffer_size())
        logging.info('resume writing')
        
    # have yet to figure out how to call this automatically
    def sender(self):
        self.checkQueue(self.alarms_  ,  10)
        self.checkQueue(self.commands_,  50)
        self.checkQueue(self.data_    , 100)
    
    def checkQueue(queue, timeout):
        if len(queue) > 0:
            if int(round(time.time() * 1000)) > (self.timeLastTransmission_ + timeout):
                self.send(queue[0])

    def receiver(self, data):
        for byte in data:
            byte = bytes([byte])
            # TODO: this could be written in more pythonic way
            # force read byte by byte
            self.received_.append(byte)
            logging.debug(byte)
            if not self.foundStart_ and byte == bytes([0x7E]):
                self.foundStart_    = True
                self.receivedStart_ = len(self.received_)
            elif byte == bytes([0x7E]) :
                decoded = self.decoder(self.received_, self.receivedStart_)
                if decoded is not None:
                    logging.debug("Preparing ACK")
                    self.send(commsFormat.commsACK(address = decoded[1]))
                else:
                    logging.debug("Preparing NACK")
                    self.send(commsFormat.commsNACK(address = decoded[1]))
                
                self.received_.clear()
                
                self.foundStart_    = False
                self.receivedStart_ = -1                
        
    def registerData(self, value):
        tmpData = commsFormat.commsDATA()
        tmpData.setInformation(value)
        self.data_.append(tmpData)
        
    def send(self, comms):
        logging.debug("Sending data...")
        self.transport_.write(self.encoder(comms.getData()))

    # escape any 0x7D or 0x7E with 0x7D and swap bit 5
    def escapeByte(self, byte):
        if byte == 0x7D or byte == 0x7E:
            return [0x7D, byte ^ (1<<5)]
        else:
            return [byte]

    # encoding data according to the protocol - escape any 0x7D or 0x7E with 0x7D and swap 5 bit
    def encoder(self, data):
        try:
            stream = [escaped for byte in data[1:-1] for escaped in self.escapeByte(byte)]
            result = bytearray([data[0]] + stream + [data[-1]])
            return result
        except:
            return None
    
    # decoding data according to the defined protocol - escape any 0x7D or 0x7E with 0x7D and swap 5 bit
    def decoder(self, data, start):
        try:
            packets = data[start:-1]

            indRemove = [idx for idx in range(len(packets)) if packets[idx] == bytes([0x7D])]
            indChange = [idx+1 for idx in indRemove]

            stream = [packets[idx][0] ^ (1<<5) if idx in indChange else packets[idx][0] for idx in range(len(packets)) if idx not in indRemove]
            result = bytearray([data[start - 1][0]] + stream + [data[-1][0]])
            return result
        except:
            return None

if __name__ == "__main__" :
    # get port number for arduino, mostly for debugging
    for port in list_ports.comports():
        try:
            if "ARDUINO" in port.manufacturer.upper():
                port = port.device
        except:
            pass

    loop = asyncio.get_event_loop()
    connection = serial_asyncio.create_serial_connection(loop, commsControl, port, baudrate=115200)
    loop.run_until_complete(connection)
    loop.run_forever()
    loop.close()