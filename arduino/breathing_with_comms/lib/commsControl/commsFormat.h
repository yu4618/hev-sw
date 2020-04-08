#ifndef COMMSFORMAT_H
#define COMMSFORMAT_H

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

    void setCounter(uint8_t counter);
    void copyData(uint8_t* data, uint8_t dataSize);

private:
    uint8_t  data_[CONST_MAX_SIZE_PACKET];
    uint8_t  packetSize_;
    uint8_t  infoSize_;
    uint16_t crc_;
};


///////////////////////////////////////////////////////////////////////////
// ALARM specific class - contains X bytes of data and specific control flag
class commsALARM: public commsFormat {
public:
    commsALARM() : commsFormat(4, PACKET_ALARM) {;} // contains 4 information bytes
};

///////////////////////////////////////////////////////////////////////////
// CMD specific class - contains X bytes of data and specific control flag
class commsCMD: public commsFormat {
public:
    commsCMD() : commsFormat(8, PACKET_CMD) {;} // contains 8 information bytes
};

///////////////////////////////////////////////////////////////////////////
// DATA specific class - contains X bytes of data
class commsDATA: public commsFormat {
public:
    commsDATA() : commsFormat(8, PACKET_DATA) {;} // contains 8 information bytes
};

///////////////////////////////////////////////////////////////////////////
// ACK specific class - contains specific control flag
class commsACK : public commsFormat {
public:
    commsACK(uint8_t address)  : commsFormat(0, address, COMMS_CONTROL_ACK << 8) {;} // contains 0 information bytes, has specific control type
};

///////////////////////////////////////////////////////////////////////////
// NACK specific class - contains specific control flag
class commsNACK: public commsFormat {
public:
  commsNACK(uint8_t address) : commsFormat(0, address, COMMS_CONTROL_NACK << 8) {;} // contains 0 information bytes, has specific control type
};
#endif // COMMSFORMAT_H
