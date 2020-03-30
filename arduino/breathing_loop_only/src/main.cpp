#include <Arduino.h>

// arduino uno
    // pwm pins
const int pin_valve_in       = 11;
const int pin_valve_out      = 6;
const int pin_valve_scavenge = 5;

    // adcs
const int pin_p_supply       = A0;
const int pin_p_regulated    = A1;
const int pin_p_buffer       = A2;
const int pin_p_inhale       = A3;
const int pin_p_exhale       = A4;

    // leds
const int pin_led_0          = 0;
const int pin_led_1          = 0;
const int pin_led_2          = 0;

    // buzzer
const int pin_buzzer         = 9;


void setup() {
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("Breathing Only");
  delay(5000);

  // buzzer 
  // tone(pin, freq (Hz), duration);
}