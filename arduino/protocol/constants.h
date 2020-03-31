#ifndef CONSTANTS_H
#define CONSTANTS_H

#define CONST_MAX_SIZE_QUEUE 16
#define CONST_MAX_SIZE_PACKET 32
#define CONST_MAX_SIZE_BUFFER 64

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
