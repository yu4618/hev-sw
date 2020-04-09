// ESP32 HUZZAH
#define BOARD "HUZZAH32"
#define HEV_FULL_SYSTEM
    // pwm pins
const int pin_valve_air_in     = 13;  // A12
const int pin_valve_o2_in      =  5;  // SCK
const int pin_valve_inhale     = 21;  // GPIO ONLY
const int pin_valve_exhale     = 19;  // MISO
const int pin_valve_purge      = 18;  // MOSI
const int pin_valve_atmosphere = 12;  // A11 / OUTPUT ONLY

    // adcs
const int pin_p_air_supply    = A4; // 36 / INPUT ONLY
const int pin_p_air_regulated = A3; // 39 / INPUT ONLY
const int pin_p_buffer        = A2; // 34 / INPUT ONLY
const int pin_p_inhale        = A0; // DAC2 / 26 ****
const int pin_p_patient       = A7; // 32
const int pin_temp            = A9; // 33
const int pin_p_o2_supply     = A10; // 27 
const int pin_p_o2_regulated  = A6;  // 14
const int pin_p_diff_patient  = A5;  // 4

    // leds
const int pin_led_0          = 17; // TX
const int pin_led_1          = 16; // RX
const int pin_led_2          = 15; // A13 ****

    // buzzer
const int pin_buzzer         = 22; // I2C SDA

    // buttons
const int pin_button_0       = 23; // I2C SCL

    // spare
// const int pin spare_adc    = A1; // 25 DAC1

// SCL / 22
// SDA / 23
// SCK / 5
// MOSI / 18
// MISO / 19
// RX / 16
// TX / 17
// A13; // ANALOG ONLY / CONNECTED TO VBAT


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
