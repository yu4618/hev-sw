#include <Arduino.h>

#if defined(ARDUINO_FEATHER_ESP32)
#define BOARD "HUZZAH32"
#include <huzzah32_pinout.h>
#elif defined(ARDUINO_NodeMCU_32S)
#define BOARD "ESP32"
#include <nodemcu_32s_pinout.h>
#elif defined(ARDUINO_AVR_UNO)
#define BOARD "UNO"
#include <Arduino_uno_pinout.h>
#elif defined(ARDUINO_SAMD_MKRVIDOR4000)
#define BOARD "VIDOR"
#include <Arduino_MKR_4000_Vidor_pinout.h>
#elif defined(ARDUINO_SAMD_MKRWIFI1010)
#define BOARD "MKR1010"
#include <Arduino_MKR_1010_WIFI_pinout.h>
#elif defined(ARDUINO_SAM_DUE)
#define BOARD "MKR1010"
#include <Arduino_Due_pinout.h>
#elif defined(ARDUINO_AVR_YUN)
#define BOARD "MKR1010"
#include <Arduino_Yun_pinout.h>
#endif


// input params
enum hev_modes : byte
{
    HEV_MODE_PS,
    HEV_MODE_CPAP,
    HEV_MODE_PRVC,
    HEV_MODE_TEST
};

enum valve_states : bool
{
    V_OPEN = LOW,
    V_CLOSED = HIGH
};

void setValves(bool vin, bool vout, bool vscav, bool vpurge);

