
// Arduino MKR 1010 Wifi
#define BOARD "MKR1010"
    // pwm pins
const int pin_valve_air_in   = 1;
const int pin_valve_o2_in    = 2;
const int pin_valve_out      = 3;
const int pin_valve_scavenge = 4;
const int pin_valve_purge    = 5;

    // adcs
const int pin_p_air_supply       = A0;
const int pin_p_air_regulated    = A1;
const int pin_p_buffer       = A2;
const int pin_p_inhale       = A3;
const int pin_p_patient      = A4;
const int pin_temp           = A5;
// const int pin_p_o2_supply       = A17; 
const int pin_p_o2_regulated    = A6;

    // leds
const int pin_led_0          = 6;
const int pin_led_1          = 7;
const int pin_led_2          = 8;

    // buzzer
const int pin_buzzer         = 0;

    // buttons
const int pin_button_0       = 1;
