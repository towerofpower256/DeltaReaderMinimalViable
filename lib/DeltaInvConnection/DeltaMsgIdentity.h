#pragma once

#include "DeltaMsgBase.h"

// TODO have a flag to say whether it's ready to read or not. Don't want to read a string from any random place in memory.

#define DELTA_MSG_IDENTITY_MIN_LEN 4
#define DELTA_MSG_IDENTITY_NAME_MAX_LEN 60

class DeltaMsgIdentity: public DeltaMsgBase
{
public:
    DeltaMsgIdentity();
    bool isReady();
    void read(uint8_t* buffer, uint8_t length);
    uint8_t variant1;
    uint8_t variant2;
    char name[DELTA_MSG_IDENTITY_NAME_MAX_LEN];
    uint8_t unk_1;

    private: 
        bool _ready = false;
};