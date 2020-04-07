#include "commsFormat.h"

// constructor to init variables
commsFormat::commsFormat(uint8_t infoSize, uint8_t address, uint16_t control) {
    memset(data_, 0, sizeof(data_));

    infoSize_   = infoSize;
    packetSize_ = infoSize + CONST_MIN_SIZE_PACKET ; // minimum size (start,address,control,fcs,stop)
    if (packetSize_ > CONST_MAX_SIZE_PACKET) {
        return;
    }

    assignBytes(getAddress(), &address, 1, false);
    assignBytes(getControl(), reinterpret_cast<uint8_t*>(&control), 2, false);

    // hardcoded defaults
    *getStart()   = COMMS_FRAME_BOUNDARY; // fixed start flag
    *getStop()    = COMMS_FRAME_BOUNDARY; // fixed stop flag

    generateCrc();
}

void commsFormat::assignBytes(uint8_t* target, uint8_t* source, uint8_t size, bool calcCrc) {
    memcpy(target, source, size);
    if (calcCrc) {
        generateCrc();
    }
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
    assignBytes(reinterpret_cast<uint8_t*>(&tmpFcs), getFcs(), 2, false);

    // return comparison
    return tmpFcs == crc_;
}

// calculate CRC value
void commsFormat::generateCrc(bool assign) {
    // calculate crc
    crc_ = uCRC16Lib::calculate(reinterpret_cast<char*>(getAddress()), static_cast<uint16_t>(infoSize_ + 3));

    // assign crc to fcs
    if (assign) {
        ;
        assignBytes(getFcs(), reinterpret_cast<uint8_t*>(&crc_), 2, false);
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

    assignBytes(getData(), data, dataSize);
}
