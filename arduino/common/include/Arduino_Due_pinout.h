// Arduino Due
#define BOARD "DUE"
#define HAS_O2
    // pwm pins
const int pin_valve_air_in   = 11;
const int pin_valve_o2_in    = 10;
const int pin_valve_out      = 6;
const int pin_valve_scavenge = 5;
const int pin_valve_purge    = 3;

    // adcs
const int pin_p_air_supply   = A0;
const int pin_p_air_regulated= A1;
const int pin_p_buffer       = A2;
const int pin_p_inhale       = A3;
const int pin_p_patient      = A4;
const int pin_temp           = A5;
const int pin_p_o2_supply    = A6; 
const int pin_p_o2_regulated = A7;
    // leds
const int pin_led_0          = 0;
const int pin_led_1          = 1;
const int pin_led_2          = 2;

    // buzzer
const int pin_buzzer         = 9;

const int pin_button_0       = 13;

    // lcd
const int pin_lcd_rs         = 22;
const int pin_lcd_en         = 24;
const int pin_lcd_d4         = 26;
const int pin_lcd_d5         = 28;
const int pin_lcd_d6         = 30;
const int pin_lcd_d7         = 32;