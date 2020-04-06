#ifndef COMMS_CONTROL_H
#define COMMS_CONTROL_H

#include <Arduino.h>
#include "Queue.h"

#include "commsConstants.h"
#include "commsFormat.h"

///////////////////////////////////////////////////////////////////////////
// class to provide simple communication protocol based on the data format
class commsControl {
public:
    commsControl(uint32_t baudrate = 115200);

    void beginSerial();

    // TODO: passed as struct, or simply array of size and memcpy? should be dataType or just define?
    void registerData(dataType type, dataFormat* values);

    void sender();
    void receiver();

private:
    DataQueue<commsFormat>* getQueue(uint8_t address);

    void sendQueue   (DataQueue<commsFormat>* queue);
    void resendPacket (DataQueue<commsFormat>* queue);
    void receivePacket(DataQueue<commsFormat>* queue);
    void finishPacket (DataQueue<commsFormat>* queue);

    bool encoder(uint8_t* data, uint8_t dataSize);
    bool decoder(uint8_t* data, uint8_t dataStart, uint8_t dataStop);

    void sendPacket(commsFormat* packet);

private:
    DataQueue<commsALARM> queueAlarm_;
    DataQueue<commsDATA>  queueData_ ;
    DataQueue<commsCMD>   queueCmd_  ;

    commsFormat commsTmp_;

    uint32_t baudrate_;

    uint64_t lastTransTime_;

    uint8_t commsReceived_[CONST_MAX_SIZE_BUFFER];
    uint8_t commsReceivedSize_;
    uint8_t commsSend_    [CONST_MAX_SIZE_BUFFER];
    uint8_t commsSendSize_;

    uint8_t lastTrans_[CONST_MAX_SIZE_BUFFER];
    uint8_t startTransIndex_;
    uint8_t lastTransIndex_;

    bool foundStart_;
};

#endif
