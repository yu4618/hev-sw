#ifndef COMMSFORMAT_H
#define COMMSFORMAT_H

// Communication protocol based on HDLC format
// author Peter Svihra <peter.svihra@cern.ch>

#include <Arduino.h>
#include <uCRC16Lib.h>

#include "commsConstants.h"

///////////////////////////////////////////////////////////////////////////
// class to provide all needed control in data format
class commsFormat {
public:
    commsFormat(uint8_t infoSize = 0, uint8_t address = 0x00, uint16_t control = 0x0000);

    uint8_t* getData()    { return data_; }
    uint8_t  getSize()    { return packetSize_; }

    void setAddress(uint8_t* address) {assignBytes(getAddress(), address, 1); }
    void setControl(uint8_t* control) {assignBytes(getControl(), control, 2); }
    void setInformation(dataFormat* values);

    void assignBytes(uint8_t* target, uint8_t* source, uint8_t size, bool calcCrc = true);

    void generateCrc(bool assign = true);
    bool compareCrc();

    // get data pointer of different parts
    uint8_t* getStart()       {return data_ + 0;}                         // starting flag of the chain
    uint8_t* getAddress()     {return data_ + 1;}                         // address where to send data, last bit is 8bit extension enable(0)/disable(1)
    uint8_t* getControl()     {return data_ + 2;}                         // frame control commands
    uint8_t* getInformation() {return data_ + 4;}                         // user information
    uint8_t* getFcs()         {return data_ + 4 + infoSize_;}             // checksum
    uint8_t* getStop()        {return data_ + 4 + infoSize_ + 2;}         // ending flag of the chain


    void setSequenceSend   (uint8_t counter);
    void setSequenceReceive(uint8_t counter);

    uint8_t getSequenceSend   ();
    uint8_t getSequenceReceive();

    void copyData(uint8_t* data, uint8_t dataSize);

    static commsFormat* generateACK()   { return new commsFormat(0, 0, COMMS_CONTROL_ACK  << 8); }
    static commsFormat* generateNACK()  { return new commsFormat(0, 0, COMMS_CONTROL_NACK << 8); }

    static commsFormat* generateALARM() { return new commsFormat(4, PACKET_ALARM); }
    static commsFormat* generateCMD()   { return new commsFormat(8, PACKET_CMD  ); }
    static commsFormat* generateDATA()  { return new commsFormat(8, PACKET_DATA ); }

private:
    uint8_t  data_[CONST_MAX_SIZE_PACKET];
    uint8_t  packetSize_;
    uint8_t  infoSize_;
    uint16_t crc_;
};

#endif // COMMSFORMAT_H
