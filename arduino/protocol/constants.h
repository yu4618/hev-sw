#ifndef CONSTANTS_H
#define CONSTANTS_H

#define CONST_MAX_SIZE_QUEUE 16
#define CONST_MAX_SIZE_PACKET 32
#define CONST_MAX_SIZE_BUFFER 64

#define CONST_ESCAPE_BIT_SWAP 5

#define PACKET_TYPE  0xC0
#define PACKET_ALARM 0xC0
#define PACKET_CMD   0x80
#define PACKET_DATA  0x40

// TODO: make sensible
#define PRESSURE_PEEP_LOW  0x01
#define PRESSURE_PEEP_HIGH 0x02

// struct for all data sent
// TODO: the same for all settings?
struct dataFormat {
  uint16_t count;
  uint16_t pressure;
};

// enum of all transfer types
enum dataType {
  dataAlarm,
  dataNormal,
  dataCommand
};

#endif
