#ifndef COMMS_CONTROL_H
#define COMMS_CONTROL_H

// Communication protocol between rasp and arduino based on HDLC format
// author Peter Svihra <peter.svihra@cern.ch>

#include <Arduino.h>
#include "RingBuf.h"

#include "commsConstants.h"
#include "commsFormat.h"

///////////////////////////////////////////////////////////////////////////
// class to provide simple communication protocol based on the data format
class commsControl {
public:
    commsControl(uint32_t baudrate = 115200);
    ~commsControl();

    void beginSerial();

    // TODO: passed as struct, or simply array of size and memcpy? should be dataType or just define?
    void registerData(dataType type, dataFormat* values);

    void sender();
    void receiver();

private:
    RingBuf<commsFormat *,CONST_MAX_SIZE_QUEUE> *getQueue(uint8_t address);

    void sendQueue    (RingBuf<commsFormat *, CONST_MAX_SIZE_QUEUE> *queue);
    void resendPacket (RingBuf<commsFormat *, CONST_MAX_SIZE_QUEUE> *queue);
    void receivePacket(RingBuf<commsFormat *, CONST_MAX_SIZE_QUEUE> *queue);
    void finishPacket (RingBuf<commsFormat *, CONST_MAX_SIZE_QUEUE> *queue);

    bool encoder(uint8_t* data, uint8_t dataSize);
    bool decoder(uint8_t* data, uint8_t dataStart, uint8_t dataStop);

    void sendPacket(commsFormat* packet);

private:
    uint8_t sequenceSend_;
    uint8_t sequenceReceive_;

    commsFormat* commsAck_;
    commsFormat* commsNck_;

    RingBuf<commsFormat *, CONST_MAX_SIZE_QUEUE> *queueAlarm_;
    RingBuf<commsFormat *, CONST_MAX_SIZE_QUEUE> *queueData_;
    RingBuf<commsFormat *, CONST_MAX_SIZE_QUEUE> *queueCmd_;

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
