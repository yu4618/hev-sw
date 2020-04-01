#include <Wire.h>    // I2C control
//    #include <LiquidCrystal_I2C.h>   //LCD over I2C
// ESP32 HUZZAH
    // pwm pins
const int pin_valve_in       = 13;
const int pin_valve_out      = 12;
const int pin_valve_scavenge = 27;
const int pin_valve_purge    = 21;

    // adcs
const int pin_p_supply       = A0;
const int pin_p_regulated    = A1;
const int pin_p_buffer       = A2;
const int pin_p_inhale       = A3;
const int pin_p_patient      = A4;
const int pin_temp           = A5;

    // leds
const int pin_led_0          = 33;
const int pin_led_1          = 15;
const int pin_led_2          = 32;

    // buzzer
const int pin_buzzer         = 14;

// lcd Not enough GPIOs, we can use I2C if needed NOTE: This code was not tested through I2C
// constants needed 
// by default static DO NOT UNCOMMENT SDA AND SCL LINES
// const uint8_t SDA = 23; 
// static const uint8_t SCL = 22;

const int LCD_Address         = 0x27;
const int LCD_columns         = 16;
const int LCD_rows            = 2;
/* In case that we want to use the LCD we must
    #include <Wire.h>
    #include <LiquidCrystal_I2C.h>
    LiquidCrystal_I2C lcd(LCD_Address, LCD_columns, LCD_rows);

void setup(){
  // initialize LCD
  lcd.init();
  // turn on LCD backlight                      
  lcd.backlight();
}
void loop(){
  // set cursor to first column, first row
  lcd.setCursor(0, 0);
  // print message
  lcd.print("Hello, World!");
  delay(1000);
  // clears the display to print new message
}

*/
