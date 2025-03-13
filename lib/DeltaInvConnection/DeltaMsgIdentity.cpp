#include "DeltaMsgIdentity.h"

bool DeltaMsgIdentity::isReady() { return _ready; }

DeltaMsgIdentity::DeltaMsgIdentity() {
    variant1 = 0;
    variant2 = 0;
    unk_1 = 0;
    name[0] = 0; // Set to empty name
}

void DeltaMsgIdentity::read(uint8_t* buffer, uint8_t length) {
    if (length < DELTA_MSG_IDENTITY_MIN_LEN) {
        // TODO handle error
    }

    // Order
    // 1 - variant 1
    // 2 - variant 2
    // 3 - name, null terminated string
    // 3 + n - ???

    variant1 = buffer[0];
    variant2 = buffer[1];
    strncpy(name, (const char*)(buffer+2), DELTA_MSG_IDENTITY_NAME_MAX_LEN);
    unk_1 = buffer[length]; // Last byte

}