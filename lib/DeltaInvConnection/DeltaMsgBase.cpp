#include "DeltaMsgBase.h"

//https://stackoverflow.com/questions/15249791/combining-two-uint8-t-as-uint16-t
uint16_t DeltaMsgBase::get2ByteUInt(uint8_t msb, uint8_t lsb) {
    uint16_t val = (msb << 8) | lsb;
    return val;
}

// Apparently the code for getting a signed int is the same as getting an unsigned int.
int16_t DeltaMsgBase::get2ByteInt(uint8_t msb, uint8_t lsb) {
    int16_t val = (msb << 8) | lsb;
    return val;
}

uint16_t DeltaMsgBase::read2ByteUInt(uint8_t* buffer, uint8_t index) {
    return get2ByteUInt(buffer[index], buffer[index+1]);
}

int16_t DeltaMsgBase::read2ByteInt(uint8_t* buffer, uint8_t index) {
    return get2ByteInt(buffer[index], buffer[index+1]);
}

float DeltaMsgBase::read2ByteUIntFactor(uint8_t *buffer, uint8_t index, uint16_t factor) {
    return read2ByteUInt(buffer, index) / (0.0 + factor);
}

// https://stackoverflow.com/questions/34943835/convert-four-bytes-to-integer-using-c
uint32_t DeltaMsgBase::get4ByteUInt(uint8_t msb1, uint8_t msb2, uint8_t lsb1, uint8_t lsb2) {
    uint16_t val = (msb1 << 24) | (msb2 << 16) | lsb1 << 8 | lsb2 ;
    return val;
}

uint32_t DeltaMsgBase::read4ByteUInt(uint8_t* buffer, uint8_t index) {
    return get4ByteUInt(buffer[index], buffer[index+1], buffer[index+2], buffer[index+3]);
}

float DeltaMsgBase::read4ByteUIntFactor(uint8_t *buffer, uint8_t index, uint16_t factor) {
    return read4ByteUInt(buffer, index) / (0.0 + factor);
}

DeltaMaMi DeltaMsgBase::getMaMi(uint8_t ma, uint8_t mi) {
    return DeltaMaMi{
        .major = ma,
        .minor = mi
    };
}