#include "commsFormat.h"

// constructor to init variables
commsFormat::commsFormat(uint8_t infoSize) {
    memset(data_, 0, sizeof(data_));

    infoSize_   = infoSize;
    packetSize_ = infoSize + CONST_MIN_SIZE_PACKET ; // minimum size (start,address,control,fcs,stop)
    if (packetSize_ > CONST_MAX_SIZE_PACKET) {
        return;
    }

    // hardcoded defaults
    *getStart()   = COMMS_FRAME_BOUNDARY; // fixed start flag
    *getAddress() = 0xFF; // 0xFF means sending to all devices
    *getStop()    = COMMS_FRAME_BOUNDARY; // fixed stop flag
}

void commsFormat::setCounter(uint8_t counter) {
    *(getControl() + 2) |= (counter << 4);
}

// compare calculated and received CRC value
bool commsFormat::compareCrc() {
    // generate data crc
    generateCrc(false);

    // get crc from fcs
    uint16_t tmpFcs;
    memcpy(&tmpFcs, getFcs(), 2);

    // return comparison
    return tmpFcs == crc_;
}

// calculate CRC value
void commsFormat::generateCrc(bool assign) {
    // calculate crc
    crc_ = uCRC16Lib::calculate(reinterpret_cast<char*>(getAddress()), infoSize_ + 3);

    // assign crc to fcs
    if (assign) {
        memcpy(getFcs(), &crc_, 2);
    }
}

// assign received information to packet
// TODO: set according to TBD dataFormat
void commsFormat::setInformation(dataFormat* values) {
    // assign values to
    memcpy(getInformation(), &values->count, 2);
}

void commsFormat::copyData(uint8_t* data, uint8_t dataSize) {
    infoSize_ = dataSize - CONST_MIN_SIZE_PACKET;
    packetSize_ = dataSize;
    memset(data_,    0, sizeof(data_));
    memcpy(data_, data,     dataSize );
}
