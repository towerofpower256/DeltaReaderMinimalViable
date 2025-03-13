
#pragma once

#include <stdint.h>
#include <cstring>

// Struct for Major:Minor used in delta messages
struct DeltaMaMi {
    uint8_t major;
    uint8_t minor;
};

enum DeltaMsgStatType {
    AT_mami,
    AT_uint16,
    AT_uint16float,
    AT_int16,
    AT_str,
    AT_byte
};

// Attribute definition
struct DeltaMsgStatDef {
    const char* name;
    uint8_t index;
    DeltaMsgStatType dataType;
    uint16_t factor;
    const char* unit;
};

class DeltaMsgBase {
    public:
    virtual void read(uint8_t* buffer, uint8_t length);
    DeltaMaMi getMaMi(uint8_t ma, uint8_t mi);
    uint16_t get2ByteUInt(uint8_t msb, uint8_t lsb);
    uint16_t read2ByteUInt(uint8_t *buffer, uint8_t index);
    int16_t get2ByteInt(uint8_t msb, uint8_t lsb);
    int16_t read2ByteInt(uint8_t *buffer, uint8_t index);
    float read2ByteUIntFactor(uint8_t *buffer, uint8_t index, uint16_t factor);
    uint32_t get4ByteUInt(uint8_t msb1, uint8_t msb2, uint8_t lsb1, uint8_t lsb2);
    uint32_t read4ByteUInt(uint8_t* buffer, uint8_t index);
    float read4ByteUIntFactor(uint8_t *buffer, uint8_t index, uint16_t factor);
};