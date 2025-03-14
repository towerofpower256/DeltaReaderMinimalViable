#include "DeltaMsgStatistics63.h"

DeltaMsgStatistics63::DeltaMsgStatistics63() {

}

uint8_t DeltaMsgStatistics63::getExpectedLength() { return DELTA_MSG_STATISTICS63_SIZE; }

void DeltaMsgStatistics63::getSapPartNumber(uint8_t* buffer, char* outBuff) {
    strncpy(outBuff, (char*)buffer, DELTA_MSG_STATISTICS63_SAPPARTLEN);
}

void DeltaMsgStatistics63::getSapSerialNumber(uint8_t* buffer, char* outBuff) {
    strncpy(outBuff, (char*)(buffer+11), DELTA_MSG_STATISTICS63_SAPSERIALLEN);
}

void DeltaMsgStatistics63::getSapDateCode(uint8_t* buffer, char* outBuff) {
    strncpy(outBuff, (char*)(buffer+29), DELTA_MSG_STATISTICS63_SAPDATELEN);
}

void DeltaMsgStatistics63::getSapRevision(uint8_t* buffer, char* outBuff) {
    strncpy(outBuff, (char*)(buffer+33), DELTA_MSG_STATISTICS63_SAPREVISIONLEN);
}

DeltaMaMi DeltaMsgStatistics63::getSoftwareRevisionAcControl(uint8_t* buffer) {
    return getMaMi(buffer[35], buffer[36]);
}

DeltaMaMi DeltaMsgStatistics63::getSoftwareRevisionDcControl(uint8_t* buffer) {
    return getMaMi(buffer[37], buffer[38]);
}

DeltaMaMi DeltaMsgStatistics63::getSoftwareRevisionDisplay(uint8_t* buffer) {
    return getMaMi(buffer[39], buffer[40]);
}

DeltaMaMi DeltaMsgStatistics63::getSoftwareRevisionEnsControl(uint8_t* buffer) {
    return getMaMi(buffer[41], buffer[42]);
}

float DeltaMsgStatistics63::getSolarCurrent1(uint8_t* buffer) {
    return read2ByteUIntFactor(buffer, 43, 10);
}

uint16_t DeltaMsgStatistics63::getSolarVoltage1(uint8_t* buffer) {
    return read2ByteUInt(buffer, 45);
}

uint16_t DeltaMsgStatistics63::getSolarResistance1(uint8_t* buffer) {
    return read2ByteUInt(buffer, 47);
}

float DeltaMsgStatistics63::getSolarCurrent2(uint8_t* buffer) {
    return read2ByteUIntFactor(buffer, 49, 10);
}

uint16_t DeltaMsgStatistics63::getSolarVoltage2(uint8_t* buffer) {
    return read2ByteUInt(buffer, 51);
}

uint16_t DeltaMsgStatistics63::getSolarResistance2(uint8_t* buffer) {
    return read2ByteUInt(buffer, 53);
}

float DeltaMsgStatistics63::getAcCurrent(uint8_t* buffer) {
    return read2ByteUIntFactor(buffer, 55, 10);
}

uint16_t DeltaMsgStatistics63::getAcVoltage(uint8_t* buffer) {
    return read2ByteUInt(buffer, 57);
}

uint16_t DeltaMsgStatistics63::getAcPower(uint8_t* buffer) {
    return read2ByteUInt(buffer, 59);
}

float DeltaMsgStatistics63::getAcFreq(uint8_t* buffer) {
    return read2ByteUIntFactor(buffer, 61, 100);
}

uint16_t DeltaMsgStatistics63::getSuppliedAcEnergyWh(uint8_t* buffer) {
    return read2ByteUInt(buffer, 63);
}

uint16_t DeltaMsgStatistics63::getInverterRuntimeMins(uint8_t* buffer) {
    return read2ByteUInt(buffer, 65);
}

int16_t DeltaMsgStatistics63::getNtcDcTemp(uint8_t* buffer) {
    return read2ByteInt(buffer, 67);
}

uint16_t DeltaMsgStatistics63::getSolarInput1MovResistance(uint8_t* buffer) {
    return read2ByteUInt(buffer, 69);
}

uint16_t DeltaMsgStatistics63::getSolarInput2MovResistance(uint8_t* buffer) {
    return read2ByteUInt(buffer, 71);
}

int16_t DeltaMsgStatistics63::getNtcACTemp(uint8_t* buffer) {
    return read2ByteInt(buffer, 73);
}

float DeltaMsgStatistics63::getAcVoltageAcControl(uint8_t* buffer) {
    return read2ByteUIntFactor(buffer, 75, 10);
}

float DeltaMsgStatistics63::getAcFreqAcControl(uint8_t* buffer) {
    return read2ByteUIntFactor(buffer, 77, 100);
}

float DeltaMsgStatistics63::getDcInjectionCurrentAcControl(uint8_t* buffer) {
    return read2ByteUIntFactor(buffer, 79, 1000);
}

float DeltaMsgStatistics63::getAcVoltageEnsControl(uint8_t* buffer) {
    return read2ByteUIntFactor(buffer, 81, 10);
}

float DeltaMsgStatistics63::getAcFreqEnsControl(uint8_t* buffer) {
    return read2ByteUIntFactor(buffer, 83, 100);
}

float DeltaMsgStatistics63::getDcInjectionCurrentEnsControl(uint8_t* buffer) {
    return read2ByteUIntFactor(buffer, 85, 1000);
}

float DeltaMsgStatistics63::getSolarCurrent1Max(uint8_t* buffer) {
    return read2ByteUIntFactor(buffer, 87, 10);
}

uint16_t DeltaMsgStatistics63::getSolarVoltage1Max(uint8_t* buffer) {
    return read2ByteUInt(buffer, 89);
}

uint16_t DeltaMsgStatistics63::getSolarPower1Max(uint8_t* buffer) {
    return read2ByteUInt(buffer, 91);
}

uint16_t DeltaMsgStatistics63::getSolarResistance1Min(uint8_t* buffer) {
    return read2ByteUInt(buffer, 93);
}

uint16_t DeltaMsgStatistics63::getSolarResistance1Max(uint8_t* buffer) {
    return read2ByteUInt(buffer, 95);
}

float DeltaMsgStatistics63::getSolarCurrent2Max(uint8_t* buffer) {
    return read2ByteUIntFactor(buffer, 97, 10);
}

uint16_t DeltaMsgStatistics63::getSolarVoltage2Max(uint8_t* buffer) {
    return read2ByteUInt(buffer, 99);
}

uint16_t DeltaMsgStatistics63::getSolarPower2Max(uint8_t* buffer) {
    return read2ByteUInt(buffer, 101);
}

uint16_t DeltaMsgStatistics63::getSolarResistance2Min(uint8_t* buffer) {
    return read2ByteUInt(buffer, 103);
}

uint16_t DeltaMsgStatistics63::getSolarResistance2Max(uint8_t* buffer) {
    return read2ByteUInt(buffer, 105);
}

float DeltaMsgStatistics63::getAcCurrentMaxToday(uint8_t* buffer) {
    return read2ByteUIntFactor(buffer, 107, 10);
}

uint16_t DeltaMsgStatistics63::getAcVoltageMinToday(uint8_t* buffer) {
    return read2ByteUInt(buffer, 109);
}

uint16_t DeltaMsgStatistics63::getAcVoltageMaxToday(uint8_t* buffer) {
    return read2ByteUInt(buffer, 111);
}

uint16_t DeltaMsgStatistics63::getAcPowerMaxToday(uint8_t* buffer) {
    return read2ByteUInt(buffer, 113);
}

float DeltaMsgStatistics63::getAcFreqMinToday(uint8_t* buffer) {
    return read2ByteUIntFactor(buffer, 115, 100);
}

float DeltaMsgStatistics63::getAcFreqMaxToday(uint8_t* buffer) {
    return read2ByteUIntFactor(buffer, 117, 100);
}

float DeltaMsgStatistics63::getSuppliedAcEnergyKwh(uint8_t* buffer) {
    return read4ByteUIntFactor(buffer, 119, 10);
}

float DeltaMsgStatistics63::getInverterRuntimeHrs(uint8_t* buffer) {
    return read4ByteUIntFactor(buffer, 123, 10);
}

uint8_t DeltaMsgStatistics63::getGlobalAlarmStatus(uint8_t* buffer) {
    return buffer[127];
}

uint8_t DeltaMsgStatistics63::getDcInputStatus(uint8_t* buffer) {
    return buffer[128];
}

uint8_t DeltaMsgStatistics63::getDcInputLimits(uint8_t* buffer) {
    return buffer[129];
}

uint8_t DeltaMsgStatistics63::getAcOutputStatus(uint8_t* buffer) {
    return buffer[130];
}

uint8_t DeltaMsgStatistics63::getAcOutputLimits(uint8_t* buffer) {
    return buffer[131];
}

uint8_t DeltaMsgStatistics63::getIsolationWarningStatus(uint8_t* buffer) {
    return buffer[132];
}

uint8_t DeltaMsgStatistics63::getDcHardwareFailure(uint8_t* buffer) {
    return buffer[133];
}

uint8_t DeltaMsgStatistics63::getAcHardwareFailure(uint8_t* buffer) {
    return buffer[134];
}

uint8_t DeltaMsgStatistics63::getEnsHardwareFailure(uint8_t* buffer) {
    return buffer[135];
}

uint8_t DeltaMsgStatistics63::getInternalBulkFailure(uint8_t* buffer) {
    return buffer[136];
}

uint8_t DeltaMsgStatistics63::getInternalCommsFailure(uint8_t* buffer) {
    return buffer[137];
}

uint8_t DeltaMsgStatistics63::getAcHardwareDisturbance(uint8_t* buffer) {
    return buffer[138];
}

// The size of the message history object
size_t DeltaMsgStatistics63::getStatusMsgHistoryLen() {
    return 20;
}

// Just dump it into a waiting buffer
void DeltaMsgStatistics63::getStatusMsgHistory(uint8_t* buffer, uint8_t* outBuffer) {
    memcpy(buffer+139, outBuffer, getStatusMsgHistoryLen());
}

void DeltaMsgStatistics63::read(uint8_t* buffer, uint8_t length) {
    if (length != DELTA_MSG_STATISTICS63_SIZE) {
        // TODO handle invalid size
    }

    // TODO
}