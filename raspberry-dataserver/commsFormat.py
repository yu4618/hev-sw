#!/usr/bin/env python3

# Communication protocol based on HDLC format
# author Peter Svihra <peter.svihra@cern.ch>

import libscrc

def commsFromBytes(byteArray):
    comms = commsFormat()
    comms.copyBytes(byteArray)
    
    return comms

# basic format based on HDLC
class commsFormat:
    def __init__(self, infoSize = 0, address = 0x00, control = [0x00, 0x00]):
        self.data_ = bytearray(7 + infoSize)
        self.infoSize_ = infoSize
        self.crc_ = None
        
        self.assignBytes(self.getStart()  , bytes([0x7E])   , calcCrc = False)
        self.assignBytes(self.getAddress(), bytes([address]), calcCrc = False)
        self.assignBytes(self.getControl(), bytes(control)  , calcCrc = False)
        self.assignBytes(self.getStop()   , bytes([0x7E])   , calcCrc = False)
        
        self.generateCrc()
        
    def getStart(self):
        return 0
    def getAddress(self):
        return 1
    def getControl(self):
        return 2
    def getInformation(self):
        return 4
    def getFcs(self):
        return 4 + self.infoSize_
    def getStop(self):
        return 4 + self.infoSize_ + 2
    
    def assignBytes(self, start, values, calcCrc = True):
        for idx in range(len(values)):
            self.data_[start + idx] = values[idx]
        if calcCrc:
            self.generateCrc()
        
    # generate checksum
    def generateCrc(self, assign = True):
        self.crc_ = libscrc.x25(bytes(self.data_[self.getAddress():self.getFcs()])).to_bytes(2, byteorder='little')
        if assign:
            self.assignBytes(self.getFcs(), self.crc_, calcCrc = False)
            
    def compareCrc(self):
        self.generateCrc(False)
        fcs = self.getData()[self.getFcs():self.getFcs()+2]
        return self.crc_ in fcs
    
    def setInformation(self, value, size = 2):
        # convert provided value
        self.assignBytes(self.getInformation(), value.to_bytes(size, byteorder='little'))
    
    def getData(self):
        return self.data_

    def copyData(self, dataArray):
        self.copyBytes(dataArray.to_bytes(self.infoSize_, byteorder='little'))
        
    def copyBytes(self, bytesArray):
        self.infoSize_ = len(bytesArray) - 7
        self.data_     = bytesArray
        
# DATA specific formating
class commsDATA(commsFormat):
    def __init__(self):
        super().__init__(infoSize = 8, address = 0x40)

# CMD specific formating
class commsCMD(commsFormat):
    def __init__(self):
        super().__init__(infoSize = 8, address = 0x80)

# ALARM specific formating
class commsALARM(commsFormat):
    def __init__(self):
        super().__init__(infoSize = 4, address = 0xC0)

# ACK specific formating
class commsACK(commsFormat):
    def __init__(self, address):
        super().__init__(control = [0x00, 0x01], address = address)
        
# NACK specific formating
class commsNACK(commsFormat):
    def __init__(self, address):
        super().__init__(control = [0x00, 0x05], address = address)
