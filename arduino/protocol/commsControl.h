#ifndef COMMS_CONTROL_H
#define COMMS_CONTROL_H

#include <Queue.h>
#include <uCRC16Lib.h>

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

  void setCounter(uint8_t counter);

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
  *getStop()    = 0x7E; // fixed stop flag
}

void commsFormat::setCounter(uint8_t counter) {
  *(getControl() + 2) |= (counter << 4);
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
  commsDATA() : commsFormat(8) { *(getControl()) = 0x40; } // contains 8 information bytes
};

///////////////////////////////////////////////////////////////////////////
// ALARM specific class - contains X bytes of data and specific control flag
class commsALARM: public commsFormat {
public:
  commsALARM() : commsFormat(4) { *(getControl()+1) = 0xC0; *(getControl()+1) = 0x02; } // contains 8 information bytes
};

///////////////////////////////////////////////////////////////////////////
// CMD specific class - contains X bytes of data and specific control flag
class commsCMD: public commsFormat {
public:
  commsCMD() : commsFormat(8) { *(getControl()) = 0x80; } // contains 8 information bytes
};

///////////////////////////////////////////////////////////////////////////
// ACK specific class - contains specific control flag
class commsACK : public commsFormat {
public:
  commsACK()  : commsFormat() { *(getControl()+1) = 0x01; } // contains 0 information bytes, has specific control type
};

///////////////////////////////////////////////////////////////////////////
// NACK specific class - contains specific control flag
class commsNACK: public commsFormat {
public:
  commsNACK() : commsFormat() { *(getControl()+1) = 0x05; } // contains 0 information bytes, has specific control type
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
  void sendPacket(DataQueue<commsFormat>* queue);
  void resendPacket(DataQueue<commsFormat>* queue);
  bool encoder(uint8_t* data, uint8_t dataSize);

  void receiver();
  void receivePacket(DataQueue<commsFormat>* queue);
  bool decoder(uint8_t* data, uint8_t dataStart, uint8_t dataStop);

private:
  DataQueue<commsFormat>* getQueue(uint8_t fmt);
  
private:
  DataQueue<commsALARM> queueAlarm_;
  DataQueue<commsDATA> queueData_;
  DataQueue<commsCMD> queueCmd_;

  uint32_t baudrate_;

  uint64_t lastTransTime_;

  uint8_t dataReceived_[CONST_MAX_SIZE_BUFFER];
  uint8_t dataReceivedSize_;
  uint8_t dataSend_    [CONST_MAX_SIZE_BUFFER];
  uint8_t dataSendSize_;

  uint8_t lastTrans_[CONST_MAX_SIZE_BUFFER];
  uint8_t startTransIndex_;
  uint8_t lastTransIndex_;

  bool foundStart_;
};

commsControl::commsControl(uint32_t baudrate) {
  baudrate_ = baudrate;
  
  lastTransTime_ = millis();

  startTransIndex_  = 0xFF;
  lastTransIndex_   = 0;
  dataReceivedSize_ = 0;
  dataSendSize_     = 0;
  memset(lastTrans_   , 0, sizeof(lastTrans_   ));
  memset(dataReceived_, 0, sizeof(dataReceived_));
  memset(dataSend_    , 0, sizeof(dataSend_    ));
  foundStart_ = false;
  
  queueAlarm_ = DataQueue<commsALARM>(CONST_MAX_SIZE_QUEUE);
  queueData_  = DataQueue<commsDATA> (CONST_MAX_SIZE_QUEUE);
  queueCmd_   = DataQueue<commsCMD>  (CONST_MAX_SIZE_QUEUE);
}

void commsControl::beginSerial() {
  Serial.begin(baudrate_);  
}

// main function to always call and try and send data
// TODO: needs switch on data type with global timeouts on data pushing
void commsControl::sender() {
  if (millis() > lastTransTime_ + 5 ) {
    sendPacket(reinterpret_cast<DataQueue<commsFormat>*>(&queueAlarm_));
  }
  
  if (millis() > lastTransTime_ + 50 ) {
    sendPacket(reinterpret_cast<DataQueue<commsFormat>*>(&queueData_));
  }
}

// main function to always try to receive data
// TODO: needs switch on data type with global timeouts on data pushing
void commsControl::receiver() {
  uint8_t currentTransIndex;
  
  // check if any data in waiting
  if (Serial.available()) {
    // while able to read data - unable is -1
    while (Serial.peek() >= 0) {
      // read byte by byte, just in case the transmission is somehow blocked
      lastTransIndex_ += Serial.readBytes(lastTrans_ + lastTransIndex_, 1);
      
      // if managed to read at least 1 byte
      if (lastTransIndex_ > 0 && lastTransIndex_ < CONST_MAX_SIZE_BUFFER) {
        currentTransIndex = lastTransIndex_ - 1;

        // find the boundary of frames
        if (lastTrans_[currentTransIndex] == 0x7E) {
          // if not found start or if read the same byte as last time
          if (!foundStart_ || startTransIndex_ == currentTransIndex) {
            foundStart_ = true;            
            startTransIndex_ = currentTransIndex;
          } else {
            // if managed to decode
            if (decoder(lastTrans_, startTransIndex_, lastTransIndex_)) {

              // to decide ACK/NACK/other
              uint8_t type = dataReceived_[3];

              // to decide what kind of packets
              uint8_t fmt  = dataReceived_[2]; 
              DataQueue<commsFormat>* tmpQueue = getQueue(fmt);
              if (tmpQueue != NULL) {
                // switch on received data to know what to do - received ACK/NACK or other
                switch(type & 0x0F) {
                  case 0x05:
                    // received NACK
                    // TODO modify timeout for next sent frame
                    resendPacket(tmpQueue);
                    break;
                  case 0x01:
                    // received ACK
                    // TODO remove from FIFO
                    receivePacket(tmpQueue);
                    break;
                  default:
                    // received DATA
                    // TODO process data
                    break;
                }                            
              }
            }

            // reset the frame
            foundStart_ = false;
            lastTransIndex_ = 0;
            startTransIndex_ = 0xFF;

            // break the loop, even if more data waiting in the bus - this frame is finished
            break;            
          }
        }
      } else if (lastTransIndex_ >= CONST_MAX_SIZE_BUFFER) {
        lastTransIndex_ = 0;
      }
    }  
  }
}


// adding new values into queue
// WIP
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
  if (dataSize > 0) {
    dataSendSize_ = 0;
    uint8_t tmpVal = 0;

    dataSend_[dataSendSize_++] = data[0];
    for (uint8_t idx = 1; idx < dataSize - 1; idx++) {
      tmpVal = data[idx];
      if (tmpVal == 0x7D || tmpVal == 0x7E) {
        dataSend_[dataSendSize_++] = 0x7D;
        tmpVal ^= (1 << CONST_ESCAPE_BIT_SWAP);
      }
      dataSend_[dataSendSize_++] = tmpVal;
    }
    dataSend_[dataSendSize_++] = data[dataSize-1];
  
    return true;  
  }
  return false;
}


// general decoder of any transmission
bool commsControl::decoder(uint8_t* data, uint8_t dataStart, uint8_t dataStop) {
  // need to have more than 1 byte transferred
  if (dataStop > dataStart+1) {
    dataReceivedSize_ = 0;
    uint8_t tmpVal = 0;
    bool escaped = false;
    
    for (uint8_t idx = dataStart; idx < dataStop; idx++) {
      tmpVal = data[idx];
      if (tmpVal == 0x7D) {
        escaped = true;
      } else {
        if (escaped) {
          tmpVal ^= (1 << CONST_ESCAPE_BIT_SWAP);
          escaped = false;
        }
        dataReceived_[dataReceivedSize_++] = tmpVal;    
      }
    }
    return true;    
  }
  return false;
}

// sending anything of commsDATA format
void commsControl::sendPacket(DataQueue<commsFormat>* queue) {
  // if have data to send
  if (!queue->isEmpty()) {
    // reset sending counter
    lastTransTime_ = millis();

    // if encoded and able to write data
    if (encoder(queue->front().getData(), queue->front().getSize()) ) {
      if (Serial.availableForWrite() >= dataSendSize_) {

        // TODO define transmission counter
        // queue->front().setCounter(someValue)

        Serial.write(dataSend_, dataSendSize_);
      }
    }
  }
}

// resending the queue values
//WIP
void commsControl::resendPacket(DataQueue<commsFormat>* queue) {
  ;
}


// receiving anything of commsFormat
// WIP
void commsControl::receivePacket(DataQueue<commsFormat>* queue) {
  queue->dequeue();
}

// get link to queue according to packet format
DataQueue<commsFormat>* commsControl::getQueue(uint8_t fmt) {
  switch (fmt & PACKET_TYPE) {
    case PACKET_ALARM:
      return reinterpret_cast<DataQueue<commsFormat>*>(&queueAlarm_);
    case PACKET_CMD:
      return reinterpret_cast<DataQueue<commsFormat>*>(&queueCmd_);
    case PACKET_DATA:
      return reinterpret_cast<DataQueue<commsFormat>*>(&queueData_);
    default:
      return NULL;
  }
}

#endif
