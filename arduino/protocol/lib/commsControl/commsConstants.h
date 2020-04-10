#ifndef COMMSCONSTANTS_H
#define COMMSCONSTANTS_H

#include <Arduino.h>

#define CONST_TIMEOUT_ALARM 5
#define CONST_TIMEOUT_DATA  5000
#define CONST_TIMEOUT_CMD   50


#define CONST_MAX_SIZE_RB_RECEIVING 10
#define CONST_MAX_SIZE_RB_SENDING 5
#define CONST_MAX_SIZE_PACKET 64
#define CONST_MAX_SIZE_BUFFER 128
#define CONST_MIN_SIZE_PACKET 7

#define COMMS_FRAME_BOUNDARY 0x7E
#define COMMS_FRAME_ESCAPE   0x7D
#define COMMS_ESCAPE_BIT_SWAP 5

#define COMMS_CONTROL_INFORMATION 0x00
#define COMMS_CONTROL_SUPERVISORY 0x01

#define COMMS_CONTROL_TYPES 0x0F
#define COMMS_CONTROL_ACK   0x00 | COMMS_CONTROL_SUPERVISORY
#define COMMS_CONTROL_NACK  0x04 | COMMS_CONTROL_SUPERVISORY

#define PACKET_TYPE  0xC0
#define PACKET_ALARM 0xC0
#define PACKET_CMD   0x80
#define PACKET_DATA  0x40
#define PACKET_SET   0x20 //set vs get ?

// TODO: make sensible
#define PRESSURE_PEEP_LOW  0x01
#define PRESSURE_PEEP_HIGH 0x02

// struct for all data sent
struct dataFormat {
    uint8_t version;
    uint8_t size = 3;
    uint8_t count = 0;
};

struct cmdFormat {
    uint8_t version;
    uint8_t size = 2;
};

struct alarmFormat {
    uint8_t version;
    uint8_t size = 2;
};

// enum of all transfer types
enum payloadType {
    payloadData,
    payloadCmd,
    payloadAlarm,
    payloadUnset
};

// payload consists of type and information
// type is set as address in the protocol
// information is set as information in the protocol
class payload {
public:
    payload(payloadType type = payloadType::payloadUnset)  {type_ = type; data_ = nullptr; cmd_ = nullptr; alarm_ = nullptr; }
    ~payload() {unsetData(); unsetAlarm(); unsetCmd();}

    void setType(payloadType type) { type_ = type; }
    payloadType getType() {return type_; }

    // requires argument as new struct
    void setData (dataFormat   *data) { unsetAll();  data_ = new  dataFormat; memcpy( data_,  data,  data->size); }
    void setCmd  (cmdFormat     *cmd) { unsetAll();   cmd_ = new   cmdFormat; memcpy(  cmd_,   cmd,   cmd->size); }
    void setAlarm(alarmFormat *alarm) { unsetAll(); alarm_ = new alarmFormat; memcpy(alarm_, alarm, alarm->size); }

    dataFormat  *getData () {return  data_; }
    cmdFormat   *getCmd  () {return   cmd_; }
    alarmFormat *getAlarm() {return alarm_; }

    void unsetAll()   { unsetData(); unsetAlarm(); unsetCmd(); }
    void unsetData()  { if ( data_ != nullptr) { delete  data_;  data_ = nullptr; } }
    void unsetCmd()   { if (  cmd_ != nullptr) { delete   cmd_;   cmd_ = nullptr; } }
    void unsetAlarm() { if (alarm_ != nullptr) { delete alarm_; alarm_ = nullptr; } }

    void setPayload(payloadType type, void* dt) {
        setType(type);
        switch (type) {
            case payloadType::payloadData:
                setData(reinterpret_cast<dataFormat*>(dt));
                break;
            case payloadType::payloadCmd:
                setCmd(reinterpret_cast<cmdFormat*>(dt));
                break;
            case payloadType::payloadAlarm:
                setAlarm(reinterpret_cast<alarmFormat*>(dt));
                break;
            default:
                break;
        }
    }

    // returns void pointer, in case you know what to do with data
    void *getInformation() {
        switch (type_) {
            case payloadType::payloadData:
                return reinterpret_cast<void*>(getData());
            case payloadType::payloadCmd:
                return reinterpret_cast<void*>(getCmd());
            case payloadType::payloadAlarm:
                return reinterpret_cast<void*>(getAlarm());
            default:
                return nullptr;
        }
    }

    uint8_t getInformationSize()  {
        switch (type_) {
            case payloadType::payloadData:
                return getData()->size;
            case payloadType::payloadCmd:
                return getCmd()->size;
            case payloadType::payloadAlarm:
                return getAlarm()->size;
            default:
                return 0;
        }
    }

private:
    payloadType type_;

    dataFormat   *data_;
    cmdFormat     *cmd_;
    alarmFormat *alarm_;
};

#endif
