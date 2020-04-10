#include <Arduino.h>
#include "commsControl.h"

#define LED_BLUE   5
#define LED_GREEN  7
#define LED_RED    3

#define BTN 8

payload plReceive_;
payload plSend_;
commsControl comms_;
dataFormat data_;

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
  plSend_.setType(payloadType::payloadData);

  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_RED, OUTPUT);

  pinMode(BTN, INPUT);

  // initialize comms connection
  comms_.beginSerial();
}

void loop() {

  // testing increase of values to be sent
  // will only increase the count on the button release (reasons)
  currentState = digitalRead(BTN);
  if ( currentState != previousState){
    if (currentState != HIGH) {
      switchLED(LED_BLUE);
      // counter increase on button press
      data_.count += 1;
      plSend_.setData(&data_);
      // register new data in comms
      comms_.writePayload(&plSend_);
    }
    previousState = currentState;
  }

  // per cycle sender
  comms_.sender();
  // per cycle receiver
  comms_.receiver();

  // per cycle data checker - the received entry is automatically removed from the buffer
  if (comms_.readPayload(&plReceive_)) {
      if (plReceive_.getType() == payloadType::payloadData) {
          switchLED(plReceive_.getData()->count);
          plReceive_.setType(payloadType::payloadUnset);
      }
  }

}
