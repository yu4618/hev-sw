#include "common.h"

void setValves(bool vin_air, bool vin_o2, bool vinhale, 
               bool vexhale, bool vpurge, bool vatmos)
{
    digitalWrite(pin_valve_air_in, vin_air);
    digitalWrite(pin_valve_o2_in, vin_o2);
    digitalWrite(pin_valve_inhale, vinhale);
    digitalWrite(pin_valve_exhale, vexhale);
    digitalWrite(pin_valve_purge, vpurge);
    digitalWrite(pin_valve_atmosphere, vatmos);

    // save the state 
    bitWrite(valve_port_states, pin_valve_air_in, vin_air);
    bitWrite(valve_port_states, pin_valve_o2_in, vin_o2);
    bitWrite(valve_port_states, pin_valve_inhale, vinhale);
    bitWrite(valve_port_states, pin_valve_exhale, vexhale);
    bitWrite(valve_port_states, pin_valve_purge, vpurge);
    bitWrite(valve_port_states, pin_valve_atmosphere, vatmos);
}

void getValves(bool &vin_air, bool &vin_o2, bool &vinhale, 
               bool &vexhale, bool &vpurge, bool &vatmos)
{
    // read the state
    vin_air = bitRead(valve_port_states, pin_valve_air_in);
    vin_o2  = bitRead(valve_port_states, pin_valve_o2_in );
    vinhale = bitRead(valve_port_states, pin_valve_inhale);
    vexhale = bitRead(valve_port_states, pin_valve_exhale);
    vpurge  = bitRead(valve_port_states, pin_valve_purge);
    vatmos  = bitRead(valve_port_states, pin_valve_atmosphere);
}