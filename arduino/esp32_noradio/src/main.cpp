#include <Arduino.h>
#include <WiFi.h>

void setup() {
  // put your setup code here, to run once:
  // Huzzah32
  // WiFi.mode(WIFI_MODE_NULL);
  WiFi.mode(WIFI_OFF);
  btStop();
  // ADC2
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A5, INPUT);
  pinMode(A12, INPUT);
  pinMode(A11, INPUT);
  pinMode(A10, INPUT);
  pinMode(A8, INPUT);
  pinMode(A6, INPUT);
  // ADC1
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  pinMode(A4, INPUT);
  pinMode(A9, INPUT);
  pinMode(A7, INPUT);


  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  int aA0 = analogRead(A0);
  int aA1 = analogRead(A1);
  int aA5 = analogRead(A5);
  int aA12 = analogRead(A12);
  int aA11 = analogRead(A11);
  int aA10 = analogRead(A10);
  int aA8 = analogRead(A8);
  int aA6 = analogRead(A6);
  int aA2 = analogRead(A2);
  int aA3 = analogRead(A3);
  int aA4 = analogRead(A4);
  int aA9 = analogRead(A9);
  int aA7 = analogRead(A7);
  Serial.println("aA0 "+String(aA0));
  Serial.println("aA1 "+String(aA1));
  Serial.println("aA5 "+String(aA5));
  Serial.println("aA12 "+String(aA12));
  Serial.println("aA11 "+String(aA11));
  Serial.println("aA10 "+String(aA10));
  Serial.println("aA8  "+String(aA8 ));
  Serial.println("aA6  "+String(aA6 ));
  Serial.println("aA2  "+String(aA2 ));
  Serial.println("aA3  "+String(aA3 ));
  Serial.println("aA4  "+String(aA4 ));
  Serial.println("aA9  "+String(aA9 ));
  Serial.println("aA7  "+String(aA7 ));

  delay(3000);
}
