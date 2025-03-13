#pragma once

#ifndef DELTAINVCONNECTION_h
#define DELTAINVCONNECTION_h

#include <stdint.h>
#include <Stream.h>

#define STX 0x02 // Message start
#define ETX 0x03 // Message end
#define ENQ 0x05 // Master enquiry
#define ACK 0x06 // Slave acknowledgement
#define NAK 0x15 // Slave non-acknowledgement, usually an error
#define BUFFER_SIZE 300 // Max message size. Max data length is 256, so shouldn't be much longer than that.
#define DATA_OFFSET 6 // Which byte is the start of the data (7th, but starts at 0 so 6)
#define MSG_TIMEOUT_MS 500 // Milliseconds to wait for a response to a request
#define DATA_MIN_LENGTH 2 // Data length should always be +2, includes CMD and SUBCMD bytes

#define DC_EVENT_START 1 // Was waiting for a start byte, got it
#define DC_EVENT_RECEIVE 2 // Graceful end to reading message
#define DC_EVENT_TIMEOUT 3 // Started reading, but didn't end before timeout elapsed
#define DC_EVENT_UNEXPECTED_END 4 // Expected to get an end byte, but got something else
#define DC_EVENT_UNEXPECTED_PROTO 5 // When the protocol type byte is a type that's not expected.
#define DC_EVENT_INVALID_LENGTH 6 // When the length byte is invalid. Typically when it's below 2, all messages should have cmd and subcmd at minimum
#define DC_IDX_PROTO 1 // Index of the protocol byte
#define DC_IDX_ID 2 // Index of the device ID byte
#define DC_IDX_LEN 3 // Index of the data length byte
#define DC_IDX_CMD 4 // Index of the cmd byte
#define DC_IDX_SUBCMD 5 // Index of the cmd byte
#define DC_IDX_DATA 6 // Index of where the data starts (7th, but starts at 0 so 6)

class DeltaConnection {
  public:
    DeltaConnection();
    void think();
    void read(uint8_t c);
    void onMsgSent(void (*callback)(uint8_t *buffer, size_t length));
    //void onMsgRcv(void (*callback)());
    void onMsgEvent(void (*callback)(uint8_t eType));
    //void onMsgStart(void (*callback)());
    void send(uint8_t deviceID, uint8_t cmd, uint8_t subcmd, uint8_t* data, uint8_t length);
    void send(uint8_t deviceID, char cmd, char subcmd) {
        send(deviceID, cmd, subcmd, 0, 0);
      };
    uint8_t* getRcvBuffer();
    uint8_t getRcvBufferLength();
    uint8_t getDeviceId();
    uint8_t getProtoType();
    uint8_t getCmd();
    uint8_t getSubCmd();
    uint8_t getDataLen();
    uint8_t* getData();
    uint16_t calcMsgCrc();
    uint8_t getCrcL();
    uint8_t getCrcH();
    uint16_t getExpectedLength();
    void resetHasMsg();
    bool hasMsg();
    bool canSend();
    uint16_t crc(uint8_t *data, uint16_t length);
    //void crcCalc(uint8_t *crcL, uint8_t *crcH, uint8_t c);
    //void crcCalc(uint8_t *crcL, uint8_t *crcH, uint8_t *data, uint16_t length);
    uint8_t getLFromCRC(uint16_t crc);
    uint8_t getHFromCRC(uint16_t crc);

  private:
    bool _hasMsg;
    uint32_t _lastSentMsg;
    //void doMsgRcv();
    //void doMsgStart();
    void doMsgSent(uint8_t *buffer, size_t length);
    void doMsgEvent(uint8_t eType);
    //void (*_cbOnMsgStart)() = 0;
    //void (*_cbOnMsgRcv)() = 0;
    void (*_cbOnMsgSent)(uint8_t *buffer, size_t length) = 0;
    void (*_cbOnMsgEvent)(uint8_t eType) = 0;
    uint8_t _buf[BUFFER_SIZE];
    uint8_t _bufIdx;
    uint16_t _expectedMsgLength;
    bool _reading = false;
    unsigned long _readStart;
    uint8_t _dataLen;
};

#endif