#ifndef COMMSCONSTANTS_H
#define COMMSCONSTANTS_H

#include <Arduino.h>

#define CONST_MAX_SIZE_QUEUE 16
#define CONST_MAX_SIZE_PACKET 64
#define CONST_MAX_SIZE_BUFFER 128
#define CONST_MIN_SIZE_PACKET 7

#define COMMS_FRAME_BOUNDARY 0x7E
#define COMMS_FRAME_ESCAPE   0x7D
#define COMMS_ESCAPE_BIT_SWAP 5

#define COMMS_CONTROL_TYPES 0x0F
#define COMMS_CONTROL_ACK   0x01
#define COMMS_CONTROL_NACK  0x05

#define PACKET_TYPE  0xC0
#define PACKET_ALARM 0xC0
#define PACKET_CMD   0x80
#define PACKET_DATA  0x40
#define PACKET_SET   0x20 //set vs get

// TODO: make sensible
#define PRESSURE_PEEP_LOW  0x01
#define PRESSURE_PEEP_HIGH 0x02

// struct for all data sent
// TODO: the same for all settings?
// struct dataFormat {
//     uint16_t count;
//     uint16_t pressure;
// };
struct dataFormat {
    uint8_t  version = A0; // 
    uint8_t  fsm_state;
    uint16_t pressure_air_supply;
    uint16_t pressure_air_regulated;
    uint16_t pressure_o2_supply;
    uint16_t pressure_o2_regulated;
    uint16_t pressure_buffer;
    uint16_t pressure_inhale;
    uint16_t pressure_patient;
    uint16_t sys_temperature;
    uint8_t  readback_valve_air_in;
    uint8_t  readback_valve_o2_in;
    uint8_t  readback_valve_out;
    uint8_t  readback_valve_scavenge;
    uint8_t  readback_valve_purge;
    uint8_t  readback_mode; 
};

struct alarmFormat{
    uint8_t tmp;  // need to define
                    // do we do the same as dataFormat and put all alarms in one message?

};

// enum of all transfer types
enum dataType {
    dataAlarm,
    dataNormal,
    dataCommand
};

#endif
