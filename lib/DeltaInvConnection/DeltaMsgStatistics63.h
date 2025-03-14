/*
Statistics message for Solivia5.0AP-G3
Variant 34 63
*/

#pragma once

#include "DeltaMsgBase.h"

#define DELTA_MSG_STATISTICS63_SIZE 159 // Binary payload should be this size exactly
//#define DELTA_MSG_STATISTICS63_ATTRIBLEN 61 // Number of attributes
#define DELTA_MSG_STATISTICS63_LONGESTSTR 18 // Size of the longest possible string, for setting buffer sizes
#define DELTA_MSG_STATISTICS63_SAPPARTLEN 11
#define DELTA_MSG_STATISTICS63_SAPSERIALLEN 18
#define DELTA_MSG_STATISTICS63_SAPDATELEN 4
#define DELTA_MSG_STATISTICS63_SAPREVISIONLEN 2

class DeltaMsgStatistics63: public DeltaMsgBase {
    public: 
    DeltaMsgStatistics63();
    void read(uint8_t* buffer, uint8_t length);
    uint8_t getExpectedLength();

    //uint8_t data[DELTA_MSG_STATISTICS63_SIZE];

    /*
    char SapPartNumber[DELTA_MSG_STATISTICS63_SAPPARTLEN+1];
    char SapSerialNumber[DELTA_MSG_STATISTICS63_SAPSERIALLEN+1];
    char SapDateCode[DELTA_MSG_STATISTICS63_SAPDATELEN+1];
    char SapRevision[DELTA_MSG_STATISTICS63_SAPREVISIONLEN+1];
    */
    
    void getSapPartNumber(uint8_t* buffer, char* outBuff);
    void getSapSerialNumber(uint8_t* buffer, char* outBuff);
    void getSapDateCode(uint8_t* buffer, char* outBuff);
    void getSapRevision(uint8_t* buffer, char* outBuff);
    DeltaMaMi getSoftwareRevisionAcControl(uint8_t* buffer);
    DeltaMaMi getSoftwareRevisionDcControl(uint8_t* buffer);
    DeltaMaMi getSoftwareRevisionDisplay(uint8_t* buffer);
    DeltaMaMi getSoftwareRevisionEnsControl(uint8_t* buffer);
    float getSolarCurrent1(uint8_t* buffer);
    uint16_t getSolarVoltage1(uint8_t* buffer);
    uint16_t getSolarResistance1(uint8_t* buffer);
    float getSolarCurrent2(uint8_t* buffer);
    uint16_t getSolarVoltage2(uint8_t* buffer);
    uint16_t getSolarResistance2(uint8_t* buffer);
    float getAcCurrent(uint8_t* buffer);
    uint16_t getAcVoltage(uint8_t* buffer);
    uint16_t getAcPower(uint8_t* buffer);
    float getAcFreq(uint8_t* buffer);
    uint16_t getSuppliedAcEnergyWh(uint8_t* buffer); // Wh
    uint16_t getInverterRuntimeMins(uint8_t* buffer); // minutes
    int16_t getNtcDcTemp(uint8_t* buffer);
    uint16_t getSolarInput1MovResistance(uint8_t* buffer); //kOhm
    uint16_t getSolarInput2MovResistance(uint8_t* buffer); //kOhm
    int16_t getNtcACTemp(uint8_t* buffer);
    float getAcVoltageAcControl(uint8_t* buffer);
    float getAcFreqAcControl(uint8_t* buffer);
    float getDcInjectionCurrentAcControl(uint8_t* buffer);
    float getAcVoltageEnsControl(uint8_t* buffer);
    float getAcFreqEnsControl(uint8_t* buffer);
    float getDcInjectionCurrentEnsControl(uint8_t* buffer);
    float getSolarCurrent1Max(uint8_t* buffer);
    uint16_t getSolarVoltage1Max(uint8_t* buffer);
    uint16_t getSolarPower1Max(uint8_t* buffer);
    uint16_t getSolarResistance1Min(uint8_t* buffer);
    uint16_t getSolarResistance1Max(uint8_t* buffer);
    float getSolarCurrent2Max(uint8_t* buffer);
    uint16_t getSolarVoltage2Max(uint8_t* buffer);
    uint16_t getSolarPower2Max(uint8_t* buffer);
    uint16_t getSolarResistance2Min(uint8_t* buffer);
    uint16_t getSolarResistance2Max(uint8_t* buffer);
    float getAcCurrentMaxToday(uint8_t* buffer);
    uint16_t getAcVoltageMinToday(uint8_t* buffer);
    uint16_t getAcVoltageMaxToday(uint8_t* buffer);
    uint16_t getAcPowerMaxToday(uint8_t* buffer);
    float getAcFreqMinToday(uint8_t* buffer);
    float getAcFreqMaxToday(uint8_t* buffer);
    float getSuppliedAcEnergyKwh(uint8_t* buffer);
    float getInverterRuntimeHrs(uint8_t* buffer);
    uint8_t getGlobalAlarmStatus(uint8_t* buffer);
    uint8_t getDcInputStatus(uint8_t* buffer);
    uint8_t getDcInputLimits(uint8_t* buffer);
    uint8_t getAcOutputStatus(uint8_t* buffer);
    uint8_t getAcOutputLimits(uint8_t* buffer);
    uint8_t getIsolationWarningStatus(uint8_t* buffer);
    uint8_t getDcHardwareFailure(uint8_t* buffer);
    uint8_t getAcHardwareFailure(uint8_t* buffer);
    uint8_t getEnsHardwareFailure(uint8_t* buffer);
    uint8_t getInternalBulkFailure(uint8_t* buffer);
    uint8_t getInternalCommsFailure(uint8_t* buffer);
    uint8_t getAcHardwareDisturbance(uint8_t* buffer);
    size_t getStatusMsgHistoryLen();
    void getStatusMsgHistory(uint8_t* buffer, uint8_t* outBuffer);

    private:
};