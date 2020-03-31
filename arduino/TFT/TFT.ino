


#include <LiquidCrystal.h>

//Arduino pins
const int rs = 7, en = 8, d4 = 9, d5 = 10, d6 = 11, d7 = 12;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup() {
  lcd.begin(16, 2);  // Declare number of columns and rows
  Serial.begin(9600);
}

void loop() {
  // This code shows on the LCD the characters received over serial port
  if (Serial.available()) {
    delay(1000);
    lcd.clear();
    while (Serial.available() > 0) {
      lcd.write(Serial.read());
    }
  }
}
