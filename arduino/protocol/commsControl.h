#ifndef COMMS_CONTROL_H
#define COMMS_CONTROL_H

#include <Queue.h>
#include "uCRC16Lib.h"

#include "constants.h"

///////////////////////////////////////////////////////////////////////////
// class to provide all needed control in data format
class commsFormat {
public:
  commsFormat(uint8_t infoSize = 0);
  
  uint8_t* getData()    { return data_; }
  uint8_t  getSize()    { return packetSize_; }

  void setInformation(dataFormat* values);

  void generateCrc(bool assign = true);
  bool compareCrc();

  // get data pointer of different parts
  uint8_t* getStart()       {return data_ + 0;}                         // starting flag of the chain
  uint8_t* getAddress()     {return data_ + 1;}                         // address where to send data, last bit is 8bit extension enable(0)/disable(1)
  uint8_t* getControl()     {return data_ + 2;}                         // frame control commands
  uint8_t* getInformation() {return data_ + 4;}                         // user information
  uint8_t* getFcs()         {return data_ + 4 + infoSize_;}             // checksum
  uint8_t* getStop()        {return data_ + 4 + infoSize_ + 2;}         // ending flag of the chain

private:
  uint8_t  data_[CONST_MAX_SIZE_PACKET];
  uint8_t  packetSize_;
  uint8_t  infoSize_;
  uint16_t crc_;
};

// constructor to init variables
commsFormat::commsFormat(uint8_t infoSize) {
  memset(data_, 0, sizeof(data_));
  
  infoSize_   = infoSize;
  packetSize_ = infoSize + 7; // minimum size is 7 (start,address,control,fcs,stop)

  // hardcoded defaults
  *getStart()   = 0x7E; // fixed start flag
  *getAddress() = 0xFF; // 0xFF means sending to all devices
  *getStop()    = 0x7F; // fixed stop flag
}

// compare calculated and received CRC value
bool commsFormat::compareCrc() {
  // generate data crc
  generateCrc(false);

  // get crc from fcs
  uint16_t tmpFcs;
  memcpy(&tmpFcs, getFcs(), 2);

  // return comparison
  return tmpFcs == crc_;
}

// calculate CRC value
void commsFormat::generateCrc(bool assign) {
  // calculate crc
  crc_ = uCRC16Lib::calculate(reinterpret_cast<char*>(getAddress()), infoSize_ + 3);

  // assign crc to fcs
  if (assign) {
    memcpy(getFcs(), &crc_, 2);
  }
}

// assign received information to packet
// TODO: set according to TBD dataFormat
void commsFormat::setInformation(dataFormat* values) {
  // assign values to 
  memcpy(getInformation(), &values->count, 2);
}

///////////////////////////////////////////////////////////////////////////
// DATA specific class - contains X bytes of data
class commsDATA: public commsFormat {
public:
  commsDATA() : commsFormat(8) { ; } // contains 8 information bytes
};

///////////////////////////////////////////////////////////////////////////
// ACK specific class - contains specific control flag
class commsACK : public commsFormat {
public:
  commsACK()  : commsFormat() { *getControl() = 0x00; } // contains 0 information bytes, has specific control type
};

///////////////////////////////////////////////////////////////////////////
// NACK specific class - contains specific control flag
class commsNACK: public commsFormat {
public:
  commsNACK() : commsFormat() { *getControl() = 0x04; } // contains 0 information bytes, has specific control type
};



///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////




///////////////////////////////////////////////////////////////////////////
// class to provide simple communication protocol based on the data format
class commsControl {
public:
  commsControl(uint32_t baudrate = 115200);

  void beginSerial();

  void registerData(dataType type, dataFormat* values);

  void sender();
  void sendData(DataQueue<commsDATA>* queue);
  void encoder(commsFormat* data);

  void receiver();
  void receiveData(DataQueue<commsDATA>* queue);
  commsFormat* decoder();
  
private:
  DataQueue<commsDATA> queueAlarm_;
  DataQueue<commsDATA> queueData_;
  DataQueue<commsDATA> queueCmd_;

  uint32_t baudrate_;

  uint64_t lastTransTime_;

  uint8_t lastTrans_[CONST_MAX_SIZE_BUFFER];
  uint8_t lastTransIndex_;
};

commsControl::commsControl(uint32_t baudrate)
{
  baudrate_ = baudrate;
  lastTransTime_ = millis();
  lastTransIndex_ = 0;
  memset(lastTrans_, 0, sizeof(lastTrans_));
  
  queueData_  = DataQueue<commsDATA>(CONST_MAX_SIZE_QUEUE);
  queueAlarm_ = DataQueue<commsDATA>(CONST_MAX_SIZE_QUEUE);
  queueCmd_   = DataQueue<commsDATA>(CONST_MAX_SIZE_QUEUE);
}

void commsControl::beginSerial() {
  Serial.begin(baudrate_);  
}

// main function to always call and try and send data
// TODO: needs switch on data type with global timeouts on data pushing
void commsControl::sender() {
  if (lastTransTime_ < 5000 + millis()) {
    sendData(&queueData_);
  }
}

// main function to always try to receive data
// TODO: needs switch on data type with global timeouts on data pushing
void commsControl::receiver() {
  if (Serial.available()) {
    decoder();
  }
}


// adding new values into queue
void commsControl::registerData(dataType type, dataFormat* values) {
  commsDATA newValue;

  // switch on different received data types
  switch(type) {
    case dataAlarm:
      break;
    case dataNormal:
      newValue.setInformation(values);
      break;
    case dataCommand:
      break;
    default:
      break;
  }

  // calculate CRC for the new entry
  newValue.generateCrc();
  // add new entry to the queue
  queueData_.enqueue(newValue);
}


// general encoder of any transmission
void commsControl::encoder(commsFormat* data) {
  uint8_t packetSize = data->getSize();
  // send start flag
  Serial.write(data->getStart()[0]);

  // encoder to split any 5 consecutive binary 1s by 0
  // TODO: missing last byte to be transferred
  uint8_t countOnes = 0;
  uint8_t realVal = 0;
  uint8_t sendVal = 0;
  uint8_t newBit = 0;
  uint8_t testVal = 0;
  for (uint8_t idxByte = 1; idxByte < packetSize - 1; idxByte++) {
    realVal = data->getData()[idxByte];

    for (uint8_t idxBit = 0; idxBit < 8; idxBit ++) {
      testVal  = (realVal >> idxBit) & 1;

      // count ones
      if (testVal == 1) {
        countOnes++;
        if (countOnes > 5) {
          newBit++;
          countOnes = 0;
        }
      }

      // if reached 8 bits, send values
      if (newBit >= 8) {
        Serial.write(sendVal);
        newBit -= 8;
        sendVal = 0;
      }

      // add to new value
      sendVal |= (testVal << newBit++);
    }
  }

  // send end flag
  Serial.write(data->getStop()[0]);
}


// general decoder of any transmission
// WIP
commsFormat* commsControl::decoder() {
  int value;
  
  do {
    value = Serial.read();

    // start head
    if (value == 0x7E) {
      lastTransIndex_ = 0;
    }
    lastTrans_[lastTransIndex_++] = value;
    Serial.print(value, HEX);
  } while (value>=0); // || value != 0x7F);

  // stop head
  if (value == 0x7F) {
    Serial.println(lastTransIndex_, DEC);
    Serial.write(lastTrans_, lastTransIndex_);
    Serial.println();    
  }
}

// sending anything of commsDATA format
// TODO: remove dequeue to receiveData after ack only
void commsControl::sendData(DataQueue<commsDATA>* queue) {
  if (!queue->isEmpty()) {
    lastTransTime_ = millis();
    
    encoder(reinterpret_cast<commsFormat*>(&queue->front()));

    queue->dequeue();
  }
}


// receiving anything of commsDATA format
// WIP
void commsControl::receiveData(DataQueue<commsDATA>* queue) {
  ;
}

#endif
