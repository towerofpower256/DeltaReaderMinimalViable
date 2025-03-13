#include "DeltaInvConnection.h"

uint8_t DeltaConnection::getDeviceId() { return _buf[DC_IDX_ID]; }
uint8_t DeltaConnection::getProtoType() { return _buf[DC_IDX_PROTO]; }
uint8_t DeltaConnection::getDataLen() { return _buf[DC_IDX_LEN]; }
uint8_t DeltaConnection::getCmd() { return _buf[DC_IDX_CMD]; }
uint8_t DeltaConnection::getSubCmd() { return _buf[DC_IDX_SUBCMD]; }
uint8_t* DeltaConnection::getData() { return &_buf[DC_IDX_DATA]; }
uint8_t DeltaConnection::getCrcL() { return _buf[DC_IDX_CMD + getDataLen()]; } // Remember: data length includes cmd and subcmd, not just the data payload
uint8_t DeltaConnection::getCrcH() { return _buf[DC_IDX_CMD + getDataLen() + 1]; }
void DeltaConnection::resetHasMsg() {_hasMsg = false;};
bool DeltaConnection::hasMsg() { return _hasMsg;};
uint8_t* DeltaConnection::getRcvBuffer() { return _buf; };
uint8_t DeltaConnection::getRcvBufferLength() { return _bufIdx; };
uint16_t DeltaConnection::getExpectedLength() { return _expectedMsgLength; };

DeltaConnection::DeltaConnection()
{
  _reading = false;
  _lastSentMsg = 0;
  resetHasMsg();
}

// Only send if lastSent is zero (never sent yet) or give some time since sending for the response to come in.
// Remember: RS485 has no flow control or bi-directional channel.
// There is a risk of both sides sending at the same time, and corrupting each other.
bool DeltaConnection::canSend()
{
  return _lastSentMsg == 0 || (millis() - _lastSentMsg) > MSG_TIMEOUT_MS;
}

uint8_t DeltaConnection::getLFromCRC(uint16 crc) {
  return crc & 0xff; // Get 1st bit by saving only the first byte with an AND operator
}

uint8_t DeltaConnection::getHFromCRC(uint16 crc) {
  return crc >> 8; // Get the 2nd bit by shifting everything down by 8 bits / 1 byte
}

uint16_t DeltaConnection::crc(uint8_t *data, uint16_t length)
{
  // CRC-16 function from inverter documentation
  uint16_t crcVal = 0x000; //initialize to zero, not all 1's
  uint16_t index;
  uint8_t bitCount;
  // For each byte
  for (index=0; index < length; index++) {
    crcVal ^= (data[index]) & 0x00ff; //make sure only 8-bits get modified
    
    // I have no idea what's going on here. looping over the bits?
    for (bitCount=0; bitCount < 8; bitCount++) {
      if (crcVal & 0x0001) { //test before shifting
        crcVal >>= 1;
        crcVal ^= 0xA001; //reflected version of poly:0x8005
      } else {
        crcVal >>= 1;
      }
    }
  }

  return crcVal;
}

uint16_t DeltaConnection::calcMsgCrc() {
  return crc(_buf+1, 5+_bufIdx); // CRC the buffer, skipping 1st byte
}

void DeltaConnection::think()
{
  // Handle timeout
  if (_reading)
  {
    if (millis() - _readStart >= MSG_TIMEOUT_MS)
    {
      //doMsgTimeout();
      doMsgEvent(DC_EVENT_TIMEOUT);
      _reading = false;
      return;
    }
  }
}

void DeltaConnection::send(uint8_t deviceID, uint8_t cmd, uint8_t subcmd, uint8_t *data, uint8_t dataLen)
{
  //uint8_t crcL = 0;
  //uint8_t crcH = 0;
  /*
  crcCalc(&crcL, &crcH, ENQ);
  crcCalc(&crcL, &crcH, deviceID);
  crcCalc(&crcL, &crcH, dataLen+DATA_MIN_LENGTH);
  crcCalc(&crcL, &crcH, cmd);
  crcCalc(&crcL, &crcH, subcmd);
  if (dataLen > 0)
    crcCalc(&crcL, &crcH, data, dataLen);
  */

  /*
  // CRC the command, subcommand, dataLen, and data
  crc.restart();
  crc.add(cmd);
  crc.add(subcmd);
  crc.add(dataLen);
  if (dataLen != 0) crc.add(data, dataLen);
  uint16_t crcVal = crc.calc();
  */
  uint16_t msgLen = 9+dataLen;
  uint8_t buff[msgLen]; // Buffer size = 9 (min message dataLen) + data length
  buff[0] = STX;
  buff[1] = ENQ;
  buff[2] = deviceID;
  buff[3] = dataLen+DATA_MIN_LENGTH;
  buff[4] = cmd;
  buff[5] = subcmd;
  if (dataLen > 0)
    memcpy(&buff[6], data, dataLen);
  uint16_t crcVal = crc(buff+1, 5+dataLen); // CRC the buffer, skipping 1st byte


  buff[6+dataLen] = getLFromCRC(crcVal);
  buff[7+dataLen] = getHFromCRC(crcVal);
  buff[8+dataLen] = ETX;
  
  doMsgSent(buff, msgLen);

  //_ser->write(buff, msgLen);


  /*
  _ser->print(STX);
  _ser->print(ENQ);
  _ser->print((char)deviceID);
  _ser->print(cmd);
  _ser->print(subcmd);
  _ser->print((char)length);
  if (length != 0)
    _ser->write((uint8_t *)&data, length);
  // if (length != 0) _ser->write((uint8_t*) &crcVal, sizeof crcVal);
  _ser->print(crcL);
  _ser->print(crcH);
  _ser->print(ETX);
  */

  _lastSentMsg = millis();
}

// Listen to incoming serial
// Wait for STX char, then start listening
// While listening, keep adding chars to the buffer
// If reading data segment, keep reading. If incoming char is ETX, keep reading because it's part of the data segment.
// If no longer reading data, read until ETX.
void DeltaConnection::read(uint8_t c)
{
  if (!_reading)
  {
    // Not reading, wait for STX
    if (c == STX)
    {
      _reading = true;
      _bufIdx = 0; // Restart the buffer IDX
      _dataLen = 0;
      _readStart = millis();
      //_expectedLength = 0;
      _expectedMsgLength = DC_IDX_DATA; // Set the min expected length. Prevents checks for end characters before reading the data length byte.
      doMsgEvent(DC_EVENT_START);
      resetHasMsg(); // going to reuse the buffer, so don't let anything read from the buffer anymore.
    }
    else
    {
      return;
    }
  }

  if (_reading)
  {
    // Add char to buffer, but dont ++ the idx yet, some checks need to use the current position
    _buf[_bufIdx] = c;

    if (_bufIdx == DC_IDX_PROTO)
    {
      if (c != ENQ && c != ACK && c != NAK)
      {
        _reading = false;
        doMsgEvent(DC_EVENT_UNEXPECTED_PROTO);
        return;
      }
    }

    if (_bufIdx == DC_IDX_LEN)
    {
      if (c < DATA_MIN_LENGTH) {
        _reading = false;
        doMsgEvent(DC_EVENT_INVALID_LENGTH);
        return;
      } else {
        _dataLen = c - DATA_MIN_LENGTH; // DataLength = cmd + subcmd + data. All we care about is data length
        _expectedMsgLength = DC_IDX_DATA + _dataLen + 2;
      }
    }

    
    _bufIdx++;
    // Done checking the incoming char. Move the index forward.
    

    // Reached what should be the ETX at the end. If it isn't, message is invalid.
    // Remember, the CRC L or CRC H could still be the same value as ETX (0x03), so can't just keep watching for ETX char, need to find ETX where ETX is supposed to be.
    if (_bufIdx > (_expectedMsgLength))
    {
      _reading = false; // Either way, reset listener, wait for next STX
      if (c == ETX)
      {
        // Success
        doMsgEvent(DC_EVENT_RECEIVE);
      }
      else
      {
        // Should've been ETX, but it wasn't. Not good.
        doMsgEvent(DC_EVENT_UNEXPECTED_END);
      }
    }
    // Reached what should be the end of the message. _bufIdx should never go beyond the expected end of here.
  }
}

void DeltaConnection::doMsgSent(uint8_t *buffer, size_t length)
{
  if (_cbOnMsgSent)
    _cbOnMsgSent(buffer, length);
}

void DeltaConnection::onMsgSent(void (*callback)(uint8_t *buffer, size_t length))
{
  _cbOnMsgSent = callback;
}

/*
void DeltaConnection::doMsgRcv()
{
  if (_cbOnMsgRcv)
    _cbOnMsgRcv();
}

void DeltaConnection::onMsgRcv(void (*callback)())
{
  _cbOnMsgRcv = callback;
}
*/

void DeltaConnection::doMsgEvent(uint8_t eType)
{
  if (_cbOnMsgEvent)
    _cbOnMsgEvent(eType);
}

void DeltaConnection::onMsgEvent(void (*callback)(uint8_t eType))
{
  _cbOnMsgEvent = callback;
}