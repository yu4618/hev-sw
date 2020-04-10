#include "common.h"

void setValves(bool vin_air, bool vin_o2, bool vinhale, bool vexhale, bool vpurge)
{

    digitalWrite(pin_valve_air_in, vin_air);
    digitalWrite(pin_valve_o2_in, vin_o2);
    digitalWrite(pin_valve_inhale, vinhale);
    digitalWrite(pin_valve_exhale, vexhale);
    digitalWrite(pin_valve_purge, vpurge);
}

void getValves(bool &vin_air, bool &vin_o2, bool &vinhale, bool &vexhale, bool &vpurge)
{
    vin_air = bitRead(PORTD, pin_valve_air_in);
    vin_o2  = bitRead(PORTD, pin_valve_o2_in );
    vinhale = bitRead(PORTD, pin_valve_inhale);
    vexhale = bitRead(PORTD, pin_valve_exhale);
    vpurge  = bitRead(PORTD, pin_valve_purge);
}