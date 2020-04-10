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

#define HEV_FORMAT_VERSION 0xA0

enum command_codes {CMD_START = 1, 
                    CMD_STOP  = 2};

// struct for all data sent

struct dataFormat {
    uint8_t  version = HEV_FORMAT_VERSION; // 
    uint8_t  size = 27; // in bytes (includes version and size itself)
    uint8_t  fsm_state = 0;
    uint16_t pressure_air_supply = 0;
    uint16_t pressure_air_regulated = 0;
    uint16_t pressure_o2_supply = 0;
    uint16_t pressure_o2_regulated = 0;
    uint16_t pressure_buffer = 0;
    uint16_t pressure_inhale = 0;
    uint16_t pressure_patient = 0;
    uint16_t temperature_buffer = 0;
    uint16_t pressure_diff_patient = 0;
    uint8_t  readback_valve_air_in = 0;
    uint8_t  readback_valve_o2_in = 0;
    uint8_t  readback_valve_inhale = 0;
    uint8_t  readback_valve_exhale = 0;
    uint8_t  readback_valve_purge = 0;
    uint8_t  readback_mode = 0; 
};

struct commandFormat {
    uint8_t  version = HEV_FORMAT_VERSION; // 
    uint8_t  size    = 7; // in bytes (includes version and size itself)
    uint8_t  cmdCode = 0;
    uint32_t  param   = 0;
};

struct alarmFormat{
    uint8_t  version = HEV_FORMAT_VERSION; // 
    uint8_t  size    = 7; // in bytes (includes version and size itself)
    uint8_t  alarmCode = 0;
    uint32_t param   = 0;
                    // do we do the same as dataFormat and put all alarms in one message?
};

// enum of all transfer types
enum dataType {
    dataAlarm,
    dataNormal,
    dataCommand
};

#endif
