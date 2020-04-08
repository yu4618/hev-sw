#include <Arduino.h>
#include "commsControl.h"
#include "test_hw_loop.h"
#include "common.h"

int ventilation_mode = HEV_MODE_PS;
float working_pressure = 1;             //?
float inspiratory_minute_volume = 6000; // ml/min
float respiratory_rate = 15;            //  10-40 +-1 ;aka breaths_per_min
float inspiratory_pressure = 10;        // 10-80 cmH2O +-1
//float tidal_volume = 200; // calc 200-1500ml +- 100
float inspiratory_time = 1.0; // 0.4-1.5s +-0.1
float pause_time = 1.0;       // range?
//float expiratory_time ; // calc
float expiratory_minute_volume; // calc?? same as inspiratory_minute_volume?
float trigger_sensitivity;

// calculations
float calc_tidal_volume()
{
    return inspiratory_minute_volume / respiratory_rate;
}

float calc_expiration_time()
{
    float total_respiratory_time = 60.0 / respiratory_rate;
    // total = expire + inspire + pause
    return (total_respiratory_time - inspiratory_time - pause_time);
}

float calc_expiratory_minute_volume()
{
    // probably need to calculate this from readings
    return 0;
}

// FROM OVERLEAF DOC
// Working pressure, corresponding to the manually set and monitored input pressure to the unit
// Inspiratory Minute Volume setting
// Breaths per minute setting
// Tidal volume display, based on previous two parameters
// Inspiration time setting
// Pause time setting
// expiration time display based on previous two parameters
// expired minute volume
// Airway pressure display based on the reading of $P_2$ ??? not a setting
// PEEP setting ??? external
// Trigger sensitivity to patient initiated breath

dataFormat data;
commsControl comms;

void setup()
{

    pinMode(pin_valve_air_in, OUTPUT);
    pinMode(pin_valve_o2_in, OUTPUT);
    pinMode(pin_valve_inhale, OUTPUT);
    pinMode(pin_valve_exhale, OUTPUT);
    pinMode(pin_valve_purge, OUTPUT);
    pinMode(pin_valve_atmosphere, OUTPUT);

    pinMode(pin_p_air_supply, INPUT);
    pinMode(pin_p_air_regulated, INPUT);
    pinMode(pin_p_buffer, INPUT);
    pinMode(pin_p_inhale, INPUT);
    pinMode(pin_p_patient, INPUT);
    pinMode(pin_temp, INPUT);
#ifdef HEV_FULL_SYSTEM
    pinMode(pin_p_o2_supply, INPUT);
    pinMode(pin_p_o2_regulated, INPUT);
    pinMode(pin_p_diff_patient, INPUT);
#endif

    pinMode(pin_led_0, OUTPUT);
    pinMode(pin_led_1, OUTPUT);
    pinMode(pin_led_2, OUTPUT);

    pinMode(pin_buzzer, OUTPUT);
    pinMode(pin_button_0, INPUT);

    comms.beginSerial();

}

void loop()
{

    // buzzer
    // tone(pin, freq (Hz), duration);

    data.fsm_state = getFsmState();
    data.readback_mode = getLabCycleMode();
    data.pressure_air_regulated = 0x1234;
    data.pressure_buffer        = 0xabcd; 

    //breath_cycle();
    FSM_assignment();
    FSM_breath_cycle();
   

    comms.registerData(dataNormal, &data);
    // per cycle sender
    comms.sender();
    // per cycle receiver
    comms.receiver();

    delay(10);
}
