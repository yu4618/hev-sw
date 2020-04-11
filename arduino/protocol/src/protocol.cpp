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
alarmFormat alarm_;

int currentState_  = 0;
int previousState_ = 0;

int led_ = 0;
bool blue_ = false;
bool green_ = false;
bool red_ = false;


// dirty function to switch one of the LEDs
void switchLED(int led) {
    int state = HIGH;
    switch (led) {
        case LED_BLUE:
            if (blue_)
                state = LOW;
            blue_ = !blue_;
            break;
        case LED_RED:
            if (red_)
                state = LOW;
            red_ = !red_;
            break;
        case LED_GREEN:
            if (green_)
                state = LOW;
            green_ = !green_;
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
    currentState_ = digitalRead(BTN);
    if ( currentState_ != previousState_) {
        if (currentState_ != HIGH) {
            switchLED(LED_BLUE);
            // counter increase on button press
            data_.fsm_state += 1;
            plSend_.setData(&data_);
            // register new data in comms
            comms_.writePayload(plSend_);
        }
        previousState_ = currentState_;
    }
//    switchLED(LED_BLUE);
    // counter increase on button press
//    plSend_.getData()->fsm_state += 1;
//    comms_.writePayload(plSend_);

    // per cycle sender
    comms_.sender();
    // per cycle receiver
    comms_.receiver();

    // per cycle data checker - the received entry is automatically removed from the buffer
    if (comms_.readPayload(plReceive_)) {

        switch (plReceive_.getType()) {
            case payloadType::payloadCmd:
                switchLED(plReceive_.getCmd()->cmdCode);
                alarm_.alarmCode = plReceive_.getCmd()->cmdCode + 1;
                plSend_.setAlarm(&alarm_);
                comms_.writePayload(plSend_);
                break;
            default:
                break;
        }

        plReceive_.setType(payloadType::payloadUnset);
    }
    delay(50);
}
