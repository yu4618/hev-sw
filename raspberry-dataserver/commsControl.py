#!/usr/bin/env python3

# Communication protocol between rasp and arduino based on HDLC format
# author Peter Svihra <peter.svihra@cern.ch>

import serial
from serial.tools import list_ports

import threading
import time

import commsFormat
import commsConstants
from collections import deque

import binascii
import logging
logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')


# communication class that governs talking between devices
class commsControl():
    def __init__(self, port, baudrate = 115200, queueSizeReceive = 16, queueSizeSend = 16):
        
        self.serial_ = None
        self.openSerial(port, baudrate)

        # send queues are FIFO ring-buffers of the defined size
        self.alarms_   = deque(maxlen = queueSizeSend)
        self.commands_ = deque(maxlen = queueSizeSend)
        self.data_     = deque(maxlen = queueSizeSend)
        
        # received queue and observers to be notified on update
        self.payloadrecv_ = deque(maxlen = queueSizeReceive)
        self.observers_ = []
        
        # needed to find packet frames
        self.received_ = []
        self.foundStart_ = False
        self.timeLastTransmission_ = int(round(time.time() * 1000))
        
        # packet counter checker
        self.sequenceSend_    = 0
        self.sequenceReceive_ = 0
        
        # initialize of the multithreading
        self.lock_ = threading.Lock()
        self.receiving_ = True
        receivingWorker = threading.Thread(target=self.receiver, daemon=True)
        receivingWorker.start()
        
        self.sending_ = True
        self._datavalid = threading.Event()  # callback for send process
        self._dvlock = threading.Lock()      # make callback threadsafe
        sendingWorker = threading.Thread(target=self.sender, daemon=True)
        sendingWorker.start()
    
    # open serial port
    def openSerial(self, port, baudrate = 115200, timeout = 2):
        if port is not None:
            self.serial_ = serial.Serial(port = port, baudrate=baudrate, timeout = timeout)
        else:
            try:
                self.serial_.close()
            except:
                logging.warning("Serial device not open")
            self.serial_ = None
        
    def sender(self):
        while self.sending_:
            self._datavalid.wait()
            if self.serial_ is not None:
                if not self.serial_.in_waiting > 0:
                    self.sendQueue(self.alarms_  ,  10)
                    self.sendQueue(self.commands_,  50)
                    self.sendQueue(self.data_    , 200)
            with self._dvlock:
                self._datavalid.clear()

    def receiver(self):
        while self.receiving_:
            if self.serial_ is not None:
                if self.serial_.in_waiting > 0:
                    with self.lock_:
                        logging.debug("Receiving data...")
                        data = self.serial_.read(self.serial_.in_waiting)
                        self.processPacket(data)

    def sendQueue(self, queue, timeout):
        if len(queue) > 0:
            logging.debug(f'Queue length: {len(queue)}')
            currentTime = int(round(time.time() * 1000))
            if currentTime > (self.timeLastTransmission_ + timeout):
                with self.lock_:
                    self.timeLastTransmission_ = currentTime
                    queue[0].setSequenceSend(self.sequenceSend_)
                    self.sendPacket(queue[0])
                    
    def getQueue(self, payloadType):
        if   payloadType == commsConstants.payloadType.payloadAlarm:
            return self.alarms_
        elif payloadType == commsConstants.payloadType.payloadCmd:
            return self.commands_
        elif payloadType == commsConstants.payloadType.payloadData:
            return self.data_
        else:
            return None
    
    def getInfoType(self, address):
        address &= 0xC0
        if address == 0xC0:
            return commsConstants.payloadType.payloadAlarm
        elif address == 0x80:
            return commsConstants.payloadType.payloadCmd
        elif address == 0x40:
            return commsConstants.payloadType.payloadData
        else:
            return commsConstants.payloadType.payloadUnset

    def processPacket(self, data):
        for byte in data:
            byte = bytes([byte])
            # TODO: this could be written in more pythonic way
            # force read byte by byte
            self.received_.append(byte)
#             logging.debug(byte)
            # find starting flag of the packet
            if not self.foundStart_ and byte == bytes([0x7E]):
                self.foundStart_    = True
                self.receivedStart_ = len(self.received_)
            # find ending flag of the packet
            elif byte == bytes([0x7E]) :
                decoded = self.decoder(self.received_, self.receivedStart_)
                if decoded is not None:
                    logging.debug(binascii.hexlify(decoded))
                    tmpComms = commsFormat.commsFromBytes(decoded)
                    if tmpComms.compareCrc():
                        control     = tmpComms.getData()[tmpComms.getControl()+1]
                        self.sequenceReceive_ = (tmpComms.getData()[tmpComms.getControl()] >> 1) & 0x7F
                        
                        # get type of payload and corresponding queue
                        payloadType = self.getInfoType(tmpComms.getData()[tmpComms.getAddress()])
                        tmpQueue   = self.getQueue(payloadType)

                        # get type of packet
                        ctrlFlag    = control & 0x0F
                        if ctrlFlag == 0x05:
                            logging.debug("Received NACK")
                            # received NACK
                        elif ctrlFlag == 0x01:
                            logging.debug("Received ACK")
                            # received ACK
                            self.finishPacket(tmpQueue)
                        else:
                            sequenceReceive = ((control >> 1) & 0x7F) + 1
                            address = tmpComms.getData()[tmpComms.getAddress():tmpComms.getControl()]

                            #payload = tmpComms.getData()[tmpComms.getInformation() : tmpComms.getFcs()]
                            
                            if self.receivePacket(payloadType, tmpComms):
                                logging.debug("Preparing ACK")
                                commsResponse = commsFormat.commsACK(address = address[0])
                            else:
                                logging.debug("Preparing NACK")
                                commsResponse = commsFormat.commsNACK(address = address)
                            commsResponse.setSequenceReceive(sequenceReceive)
                            self.sendPacket(commsResponse)
                    
                self.received_.clear()
                
                self.foundStart_    = False
                self.receivedStart_ = -1        
        
    def writePayload(self, payload):
        payloadType = payload.getType()
        if   payloadType == commsConstants.payloadType.payloadAlarm:
            tmpComms = commsFormat.generateAlarm(payload)
        elif payloadType == commsConstants.payloadType.payloadCmd:
            tmpComms = commsFormat.generateCmd(payload)
        elif payloadType == commsConstants.payloadType.payloadData:
            tmpComms = commsFormat.generateData(payload)
        else:
            return False        
        tmpComms.setInformation(payload)
        
        tmpQueue = self.getQueue(payloadType)
        tmpQueue.append(tmpComms)
        with self._dvlock:
            self._datavalid.set()
            
        return True
        
    def sendPacket(self, comms):
        logging.debug("Sending data...")
        logging.debug(binascii.hexlify(self.encoder(comms.getData())))
        self.serial_.write(self.encoder(comms.getData()))
    
    def finishPacket(self, queue):
        try:
            if len(queue) > 0:
                # 0x7F to deal with possible overflows (0 should follow after 127)
                if ((queue[0].getSequenceSend() + 1) & 0x7F) == self.sequenceReceive_:
                    self.sequenceSend_ = (self.sequenceSend_ + 1) % 128
                    queue.popleft()
        except:
            logging.debug("Queue is probably empty")
            
    def receivePacket(self, payloadType, commsPacket):
        if   payloadType == commsConstants.payloadType.payloadAlarm:
            payload = commsConstants.alarmFormat()
        elif payloadType == commsConstants.payloadType.payloadCmd:
            payload = commsConstants.commandFormat()
        elif payloadType == commsConstants.payloadType.payloadData:
            payload = commsConstants.dataFormat()
        else:
            return False
        
        payload.fromByteArray(commsPacket.getData()[commsPacket.getInformation():commsPacket.getFcs()])
        self.payloadrecv = payload
        return True

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
        
    # callback to dependants to read the received payload
    @property
    def payloadrecv(self):
        return self.payloadrecv_

    @payloadrecv.setter
    def payloadrecv(self, payload):
        self.payloadrecv_.append(payload)
        logging.debug(f"Pushed {payload} to FIFO")
        for callback in self.observers_:
            # peek at the leftmost item, don't pop until receipt confirmed
            callback(self.payloadrecv_[0])

    def bind_to(self, callback):
        self.observers_.append(callback)

    def pop_payloadrecv(self):
        # from callback. confirmed receipt, pop value
        poppedval = self.payloadrecv_.popleft()
        logging.debug(f"Popped {poppedval} from FIFO")
        if len(self.payloadrecv_) > 0:
            # purge full queue if Dependant goes down when it comes back up
            for callback in self.observers_:
                callback(self.payloadrecv_[0])
        
if __name__ == "__main__" :
    # example dependant
    class Dependant(object):
        def __init__(self, lli):
            self._llipacket = None
            self._lli = lli
            self._lli.bind_to(self.update_llipacket)

        def update_llipacket(self, payload):
#             logging.debug(f"payload received: {payload!r}")
            self._llipacket = payload
            # pop from queue - protects against Dependant going down and not receiving packets
            self._lli.pop_payloadrecv()

    # get port number for arduino, mostly for debugging
    for port in list_ports.comports():
        try:
            if "ARDUINO" in port.manufacturer.upper():
                port = port.device
        except:
            pass

    commsCtrl = commsControl(port = port)
    example = Dependant(commsCtrl)
    
    payloadSend = commsConstants.commandFormat()
    payloadSend.cmdCode = 3
    payloadSend.toByteArray()
    
    def burst(payload):
        for _ in range(16):
            commsCtrl.writePayload(payload)
    
#     commsCtrl.writePayload(payloadSend)

#     commsCtrl.payloadrecv = "testpacket1"
#     commsCtrl.payloadrecv = "testpacket2"

#     LEDs = [3,5,7]
#     for _ in range(30):
#         for led in LEDs:
#             commsCtrl.registerData(led)
#         time.sleep(5)

#     while True:
#         pass