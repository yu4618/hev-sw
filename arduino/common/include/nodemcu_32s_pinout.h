// Node MCU 32s
#define BOARD "NODEMCU32S"
#define HEV_FULL_SYSTEM
    // pwm pins
const int pin_valve_air_in        = 3;
const int pin_valve_o2_in         = 5;   // STRAPPING PIN (prefer as output / high-Z)
const int pin_valve_inhale        = 21;  // formerly valve_out
const int pin_valve_exhale        = 19;  // formerly valve_scavenge
const int pin_valve_purge         = 18;
const int pin_valve_atmosphere    = 12;// A15  // STRAPPING PIN (prefer as output / high-Z)

    // adcs
const int pin_p_air_supply       = A0; // IO36  // INPUT ONLY
const int pin_p_air_regulated    = A3; // IO39  // INPUT ONLY
const int pin_p_buffer           = A6; // IO34  // INPUT ONLY
const int pin_p_inhale           = A7; // IO35  // INPUT ONLY
const int pin_p_patient          = A4; // IO32
const int pin_temp               = A5; // IO33
const int pin_p_o2_supply       = A17; // IO27
const int pin_p_o2_regulated    = A16; // IO14 
const int pin_p_diff_patient    = A10;  // IO4

    // leds
const int pin_led_0          = 17;
const int pin_led_1          = 16;
const int pin_led_2          =  1;

    // buzzer
const int pin_buzzer         =  2;   // STRAPPING PIN (prefer as output / High-Z)

    // buttons
const int pin_button_0       = 13;

    // SPARES - this and i2c and dac are spare
const int pin_spare_adc         = A13;  // IO15  // STRAPPING PIN (prefer as output / high-Z)
    // i2c
const int pin_scl = 23;
const int pin_sda = 22;
    // dacs
const int pin_dac_1 = 25;  // A18
const int pin_dac_2 = 26;  // A19




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
