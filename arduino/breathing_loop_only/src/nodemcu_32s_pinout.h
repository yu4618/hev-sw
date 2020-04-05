#include <Wire.h>    // I2C control
//    #include <LiquidCrystal_I2C.h>   //LCD over I2C
// Node MCU 32s
    // pwm pins
const int pin_valve_air_in   = 23;
const int pin_valve_o2_in    = 22;
const int pin_valve_out      = 21;
const int pin_valve_scavenge = 19;
const int pin_valve_purge    = 18;

    // adcs
const int pin_p_air_supply       = A0; 
const int pin_p_air_regulated    = A3;
const int pin_p_buffer           = A6;
const int pin_p_inhale           = A7;
const int pin_p_patient          = A4;
const int pin_temp               = A5;
const int pin_p_o2_supply       = A17; 
const int pin_p_o2_regulated    = A16;

    // leds
const int pin_led_0          = 17;
const int pin_led_1          = 16;
const int pin_led_2          =  1;

    // buzzer
const int pin_buzzer         =  2;

    // buttons
const int pin_button_0       = 13;

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
