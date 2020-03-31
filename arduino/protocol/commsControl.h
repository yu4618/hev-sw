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
  bool encoder(uint8_t* data, uint8_t dataSize);

  void receiver();
  void receiveData(DataQueue<commsDATA>* queue);
  bool decoder(uint8_t* data, uint8_t dataSize);
  
private:
  DataQueue<commsDATA> queueAlarm_;
  DataQueue<commsDATA> queueData_;
  DataQueue<commsDATA> queueCmd_;

  uint32_t baudrate_;

  uint64_t lastTransTime_;

  uint8_t dataReceived_[CONST_MAX_SIZE_BUFFER];
  uint8_t dataReceivedSize_;
  uint8_t dataSend_    [CONST_MAX_SIZE_BUFFER];
  uint8_t dataSendSize_;

  uint8_t lastTrans_[CONST_MAX_SIZE_BUFFER];
  uint8_t lastTransIndex_;

  bool foundStart_;
};

commsControl::commsControl(uint32_t baudrate)
{
  baudrate_ = baudrate;
  
  lastTransTime_ = millis();
  
  lastTransIndex_   = 0;
  dataReceivedSize_ = 0;
  dataSendSize_     = 0;
  memset(lastTrans_   , 0, sizeof(lastTrans_   ));
  memset(dataReceived_, 0, sizeof(dataReceived_));
  memset(dataSend_    , 0, sizeof(dataSend_    ));
  foundStart_ = false;
  
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
    
    while (Serial.peek() > 0) {
      // read byte by byte
      lastTransIndex_ += Serial.readBytes(lastTrans_ + lastTransIndex_, 1);


      // TODO deal with foundStard
      
      if (foundStart_ && lastTransIndex_ > 0 && lastTrans_[lastTransIndex_-1] == 0x7E) {
        decoder(lastTrans_, lastTransIndex_);
        break;
      }
    }  
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
bool commsControl::encoder(uint8_t* data, uint8_t dataSize) {
  dataSendSize_ = 0;
  uint8_t tmpVal = 0;
  
  for (uint8_t idx = 0; idx < dataSize; idx++) {
    tmpVal = data[idx];
    if (tmpVal == 0x7D || tmpVal == 0x7E) {
      dataSend_[dataSendSize_++] = 0x7D;
      tmpVal ^= (1<<4);
    }
    dataSend_[dataSendSize_++] = tmpVal;
  }

  return true;
}


// general decoder of any transmission
bool commsControl::decoder(uint8_t* data, uint8_t dataSize) {
  dataReceivedSize_ = 0;
  uint8_t tmpVal = 0;
  bool escaped = false;
  
  for (uint8_t idx = 0; idx < dataSize; idx++) {
    tmpVal = data[idx];
    if (tmpVal == 0x7D) {
      escaped = true;
    } else {
      if (escaped) {
        tmpVal ^= (1<<4);
        escaped = false;
      }
      dataReceived_[dataReceivedSize_++] = tmpVal;    
    }
  }
  
  return true;
}

// sending anything of commsDATA format
// TODO: remove dequeue to receiveData after ack only
void commsControl::sendData(DataQueue<commsDATA>* queue) {
  if (!queue->isEmpty()) {
    lastTransTime_ = millis();
    
    if (encoder(queue->front().getData(), queue->front().getSize()) ) {
      if (Serial.availableForWrite() >= dataSendSize_) {

        Serial.write(dataSend_, dataSendSize_);    
  
        queue->dequeue();
      }
    }
  }
}


// receiving anything of commsDATA format
// WIP
void commsControl::receiveData(DataQueue<commsDATA>* queue) {
  ;
}

#endif
