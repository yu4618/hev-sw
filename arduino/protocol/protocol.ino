#include "commsControl.h"
#include "constants.h"

#define LED_BLUE   5
#define LED_GREEN  7
#define LED_RED    3

#define BTN 8

dataFormat data;
commsControl comms;

int currentState  = 0;
int previousState = 0;

int led = 0;
bool blue = false;
bool green = false;
bool red = false;

// dirty function to switch one of the LEDs
void switchLED(int led) {
  int state = HIGH;
  switch (led) {
    case LED_BLUE:
      if (blue)
        state = LOW;
      blue = !blue;
      break;
    case LED_RED:
      if (red)
        state = LOW;
      red = !red;
      break;
    case LED_GREEN:
      if (green)
        state = LOW;
      green = !green;
      break;
  }
  digitalWrite(led, state);
}


void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_RED, OUTPUT);

  pinMode(BTN, INPUT);

  // initialize comms connection
  comms.beginSerial();
  data.count = 0;
}

void loop() {

  // testing increase of values to be sent
  // will only increase the count on the button release (reasons)
  currentState = digitalRead(BTN);
  if ( currentState != previousState){
    if (currentState != HIGH) {
      switchLED(LED_BLUE);
      // counter increase on button press
      data.count += 62;

      // register new data in comms
      comms.registerData(dataNormal, &data);
    }
    previousState = currentState;
  }

  // per cycle sender
  comms.sender();
  // per cycle receiver
  comms.receiver();
}
