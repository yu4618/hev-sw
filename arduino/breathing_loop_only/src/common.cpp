#include "common.h"

void setValves(bool vin, bool vout, bool vscav, bool vpurge)
{

    digitalWrite(pin_valve_in, vin);
    digitalWrite(pin_valve_out, vout);
    digitalWrite(pin_valve_scavenge, vscav);
    digitalWrite(pin_valve_purge, vpurge);
}
