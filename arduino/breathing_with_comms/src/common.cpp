#include "common.h"

void setValves(bool vin, bool vout, bool vscav, bool vpurge)
{

    digitalWrite(pin_valve_air_in, vin);
    digitalWrite(pin_valve_o2_in, vin);
    digitalWrite(pin_valve_inhale, vout);
    digitalWrite(pin_valve_exhale, vscav);
    digitalWrite(pin_valve_purge, vpurge);
}
