#include <Arduino.h>
#include <SoftwareSerial.h>
#include <PubSubClient.h>
#include "EspLogUdp.h"
#include "DeltaInvConnection.h"
#include "secrets.h"
#include "DeltaMsgIdentity.h"
#include "DeltaMsgStatistics63.h"

#ifdef ESP32
#include <WiFi.h>
// #include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
// #include <ESPAsyncTCP.h>
#endif

#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>

// === Software Serial stuff ===

#define S_TX_PIN D6
#define S_RX_PIN D5
#define S_BAUD 19200 // Serial baud rate
EspSoftwareSerial::UART ess;

// === Logger stuff ===

EspLogUdp logger;
const char* logPrefix = "[Delta] ";

uint32_t lastPoll;
uint8_t pollState = 0;

// === Serial Echo stuff ===

#define SECHO_INTERVAL 500
#define SECHO_BUFFERSIZE 255
uint8_t sechoBuff[SECHO_BUFFERSIZE];
uint8_t sechoBuffIdx = 0;
uint32_t sechoLastEcho = 0;

// === Delta stuff ===

#define INVERTER_POLL_FREQ 1000
#define INV_ID 1

//DeltaConnection delta(&Serial);
//DeltaConnection delta(&ess);
DeltaConnection delta;

DeltaMsgIdentity msgIdentity;
DeltaMsgStatistics63 msgStatsReader;

// === MQTT stuff ===
WiFiClient espClient;
PubSubClient mqttClient(espClient);
#define MQTT_MSG_BUFFER_SIZE	(256)
char mqttMsgBuff[MQTT_MSG_BUFFER_SIZE];
#define MQTT_RECONNECT_ATTEMPT_INTERVAL 1000
uint32_t mqttLastConnAttempt = 0;


// === OTA stuff === 
//To make Arduino software autodetect OTA device
WiFiServer TelnetServer(8266);


char* bytesToHex(char* hexbuff, uint8_t val) {
  sprintf(hexbuff, "%02X", val);
  return hexbuff;
}

void logAsHex(uint8_t *buffer, size_t length) {
  char hexBuf[3];
  size_t i=0;
  for (i=0; i < length; i++) {
    uint8_t c = buffer[i];
    logger.print(bytesToHex(hexBuf, c));
    logger.print(' ');
  }
}

void sechoThink() {
  if (sechoBuffIdx && (sechoLastEcho == 0 || (millis() - sechoLastEcho) > SECHO_INTERVAL)) {
    // Echo time
    logger.start();
    logger.print("ECHO ");
    logAsHex(sechoBuff, sechoBuffIdx);
    logger.end();
    
    sechoBuffIdx = 0; // Reset buffer
    
    sechoLastEcho = millis();
  }
}

void sechoRead(uint8_t c) {
  if (sechoBuffIdx < SECHO_BUFFERSIZE) {
    sechoBuff[sechoBuffIdx] = c;
    sechoBuffIdx++;
  }
}

void sendPoll()
{
  if (pollState == 0) {
    logger.send("Req ident");
    delta.send(INV_ID, 0x00, 0x00);
    pollState = 1;
  } else if (pollState == 1) {
    logger.send("Req stats");
    delta.send(INV_ID, 0x60, 0x01);
    pollState = 0;
  }
  lastPoll = millis();
}

void invThink()
{
  // Should we send a poll to inverter?
  // If so, send one
  if (lastPoll == 0 || (millis() - lastPoll) > INVERTER_POLL_FREQ)
  {
    if (delta.canSend())
    {
      sendPoll();
    }
  }

  delta.think();
  #ifdef USE_SERIAL
  while (Serial.available()) {
    uint8_t sc = Serial.read();
    delta.read(sc);
  }
  #endif
  
  while (ess.available())
  {
    uint8_t c = ess.read();
    sechoRead(c);
    delta.read(c);
  }
}

void handleMsgSent(uint8_t *buffer, size_t length) {
  char hexBuf[3];

  // Send it
  ess.write(buffer, length);

  logger.start();
  logger.print("S> ");
  size_t i=0;
  for (i=0; i < length; i++) {
    uint8_t c = buffer[i];
    logger.print(bytesToHex(hexBuf, c));
  }
  logger.end();
}



void handleMsgTimeout() {
  char hexStr[3];
  uint8_t* buffer = delta.getRcvBuffer();
  uint8_t length = delta.getRcvBufferLength();
  logger.start();
  logger.print("TIMEOUT> ");
  size_t i=0;
  for (i=0; i < length; i++) {
    uint8_t c = buffer[i];
    logger.print(bytesToHex(hexStr, c));
  }
  logger.end();
}

void handleMsgRcv() {
  char hexStr[3];
  uint8_t* buffer = delta.getRcvBuffer();
  uint8_t length = delta.getRcvBufferLength();
  logger.start();
  logger.print("R< ");
  size_t i=0;
  for (i=0; i < length; i++) {
    uint8_t c = buffer[i];
    logger.print(bytesToHex(hexStr, c));
  }

  // Check the CRC. CRC should include the proto, address, data length, cmd, subcmd, and data
  uint16_t crc = delta.crc(buffer+DC_IDX_PROTO, length - DC_IDX_PROTO - 3); // CRC
  uint8_t crcL = delta.getLFromCRC(crc); 
  uint8_t crcH = delta.getHFromCRC(crc); 

  logger.print(" CRC ");
  if (crcL == delta.getCrcL() && crcH == delta.getCrcH()) {
    logger.print("OK ");
  } else {
    logger.print("FAIL ");
  }
  logger.print("calc ");
  logger.print(bytesToHex(hexStr, crcL));
  logger.print(' ');
  logger.print(bytesToHex(hexStr, crcH));
  logger.print(" msg ");
  logger.print(bytesToHex(hexStr, delta.getCrcL()));
  logger.print(' ');
  logger.print(bytesToHex(hexStr, delta.getCrcH()));

  logger.end();

  if (delta.getCmd() == 0 && delta.getSubCmd() == 0) {
    // Is identity response
    msgIdentity.read(delta.getData(), delta.getDataLen());
    logger.start();
    logger.print("Identity ");
    logger.print("Variant ");
    logger.printf("%02d %02d", msgIdentity.variant1, msgIdentity.variant2);
    logger.print(" Name ");
    logger.print(msgIdentity.name);
    logger.end();
  }
  if (delta.getCmd() == 96 && delta.getSubCmd() == 01) {
    //msgStatsReader.read(delta.getData(), delta.getDataLen());
    char strbuff[DELTA_MSG_STATISTICS63_LONGESTSTR];
    DeltaMaMi mamibuff;
    logger.start();
    logger.println("Stats");
    logger.print("SapPartNumber "); memset(strbuff, 0, sizeof strbuff); msgStatsReader.getSapPartNumber(delta.getData(), strbuff); logger.print(strbuff); logger.println();
    logger.print("SapSerialNumber "); memset(strbuff, 0, sizeof strbuff); msgStatsReader.getSapSerialNumber(delta.getData(), strbuff); logger.print(strbuff); logger.println();
    logger.print("SapDateCode "); memset(strbuff, 0, sizeof strbuff); msgStatsReader.getSapDateCode(delta.getData(), strbuff); logger.print(strbuff); logger.println();
    logger.print("SapRevision "); memset(strbuff, 0, sizeof strbuff); msgStatsReader.getSapRevision(delta.getData(), strbuff); logger.print(strbuff); logger.println();

    logger.print("SoftwareRevisionAcControl "); mamibuff = msgStatsReader.getSoftwareRevisionAcControl(buffer); logger.print(mamibuff.major); logger.print('.'); logger.print(mamibuff.minor); logger.println();
    logger.print("SoftwareRevisionDcControl "); mamibuff = msgStatsReader.getSoftwareRevisionDcControl(buffer); logger.print(mamibuff.major); logger.print('.'); logger.print(mamibuff.minor); logger.println();
    logger.print("SoftwareRevisionDisplay "); mamibuff = msgStatsReader.getSoftwareRevisionDisplay(buffer); logger.print(mamibuff.major); logger.print('.'); logger.print(mamibuff.minor); logger.println();
    logger.print("SoftwareRevisionEnsControl "); mamibuff = msgStatsReader.getSoftwareRevisionEnsControl(buffer); logger.print(mamibuff.major); logger.print('.'); logger.print(mamibuff.minor); logger.println();

    logger.print("SolarCurrent1 "); logger.print(msgStatsReader.getSolarCurrent1(buffer)); logger.println();
    logger.print("SolarVoltage1 "); logger.print(msgStatsReader.getSolarVoltage1(buffer)); logger.println();
    logger.print("SolarResistance1 "); logger.print(msgStatsReader.getSolarResistance1(buffer)); logger.println();
    logger.print("SolarCurrent2 "); logger.print(msgStatsReader.getSolarCurrent2(buffer)); logger.println();
    logger.print("SolarVoltage2 "); logger.print(msgStatsReader.getSolarVoltage2(buffer)); logger.println();
    logger.print("SolarResistance2 "); logger.print(msgStatsReader.getSolarResistance2(buffer)); logger.println();
    logger.print("AcCurrent "); logger.print(msgStatsReader.getAcCurrent(buffer)); logger.println();
    logger.print("AcVoltage "); logger.print(msgStatsReader.getAcVoltage(buffer)); logger.println();
    logger.print("AcPower "); logger.print(msgStatsReader.getAcPower(buffer)); logger.println();
    logger.print("AcFreq "); logger.print(msgStatsReader.getAcFreq(buffer)); logger.println();
    logger.print("SuppliedAcEnergyWh "); logger.print(msgStatsReader.getSuppliedAcEnergyWh(buffer)); logger.println();
    logger.print("SuppliedAcEnergyKwh "); logger.print(msgStatsReader.getSuppliedAcEnergyKwh(buffer)); logger.println();
    logger.print("InverterRuntimeMins "); logger.print(msgStatsReader.getInverterRuntimeMins(buffer)); logger.println();
    logger.print("InverterRuntimeHrs "); logger.print(msgStatsReader.getInverterRuntimeHrs(buffer)); logger.println();
    logger.print("NtcDcTemp "); logger.print(msgStatsReader.getNtcDcTemp(buffer)); logger.println();
    logger.print("SolarInput1MovResistance "); logger.print(msgStatsReader.getSolarInput1MovResistance(buffer)); logger.println();
    logger.print("SolarInput2MovResistance "); logger.print(msgStatsReader.getSolarInput2MovResistance(buffer)); logger.println();
    logger.print("NtcACTemp "); logger.print(msgStatsReader.getNtcACTemp(buffer)); logger.println();
    logger.end();
  }
}

void handleMsgEvent(uint8_t eType) {
  char hexStr[3];

  switch (eType) {
    case DC_EVENT_TIMEOUT:
      //handleMsgTimeout();
      logger.start();
      logger.print("Rcv Timeout Expected len: ");
      logger.print(delta.getExpectedLength());
      logger.print(" Data: ");
      logAsHex(delta.getRcvBuffer(), delta.getRcvBufferLength());
      logger.end();
      break;

    case DC_EVENT_START:
      logger.send("Rcv started");
      break;

    case DC_EVENT_RECEIVE:
      logger.send("Rcv success");
      handleMsgRcv();
      break;

    case DC_EVENT_UNEXPECTED_PROTO:
      
      logger.start();
      logger.print("Unexpected proto ");
      bytesToHex(hexStr, delta.getProtoType());
      logger.print(hexStr);
      logger.end();
      break;
      
    case DC_EVENT_UNEXPECTED_END:
      logger.start();
      logger.print("Unexpected final byte. Expected len: ");
      logger.print(delta.getExpectedLength());
      logger.print(" Data: ");
      logAsHex(delta.getRcvBuffer(), delta.getRcvBufferLength());
      logger.end();
      break;

    case DC_EVENT_INVALID_LENGTH:
      logger.start();
      logger.print("Invalid length byte ");
      bytesToHex(hexStr, delta.getDataLen());
      logger.print(hexStr);
      logger.end();
  }
}

void setupOta() {
  // Setup OTA
  // Port defaults to 8266
  //ArduinoOTA.setPort(8266);
  // Hostname defaults to esp8266-[ChipID]
  //ArduinoOTA.setHostname("myesp8266");
  // No authentication by default
  //ArduinoOTA.setPassword((const char *)"xxxxx");

  ArduinoOTA.onStart([]() {
    //Serial.println("OTA Start");
    logger.send("OTA Start");
  });
  ArduinoOTA.onEnd([]() {
    //Serial.println("OTA End");
    //Serial.println("Rebooting...");
    logger.send("OTA End. Rebooting...");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    //Serial.printf("Progress: %u%%\r\n", (progress / (total / 100)));
    logger.start();
    logger.printf("Progress: %u%%\r\n", (progress / (total / 100)));
    logger.end();
  });
  ArduinoOTA.onError([](ota_error_t error) {
    //Serial.printf("Error[%u]: ", error);
    logger.start();
    logger.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) logger.print("Auth failed"); //Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) logger.print("Begin Failed"); //Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) logger.print("Connect Failed"); //Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) logger.print("Receive Failed"); //Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) logger.print("End Failed"); //Serial.println("End Failed");
    logger.end();
  });
  ArduinoOTA.begin();
}

void setup() {
    Serial.begin(S_BAUD);
    //nss.begin(S_BAUD);
    ess.begin(S_BAUD, EspSoftwareSerial::SWSERIAL_8N1, S_RX_PIN, S_TX_PIN);
    // ESS tries to set the input pin to INPUT_PULLUP, but we're pulling the pin DOWN using resisters in a voltage divider. Override it.
    //pinMode(S_RX_PIN, INPUT);
    ess.enableRxGPIOPullUp(false);

    logger.setPrefix(logPrefix);
    logger.send("Starting");

    //To make Arduino software autodetect OTA device
    TelnetServer.begin();

    WiFi.begin(wifi_ssid, wifi_pass);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
    }

    // Setup mDNS
    if (!MDNS.begin("esp8266")) {
      logger.send("mDNS failed to start");
    }
    logger.send("mDNS started");

    // Send restart reason
    rst_info *resetInfo;
    resetInfo = ESP.getResetInfoPtr();
    logger.start();
    logger.print("Restart reason ");
    logger.print(resetInfo->reason);
    logger.print(' ');
    if (resetInfo->reason == REASON_DEFAULT_RST) logger.print("DEFAULT_RST");
    if (resetInfo->reason == REASON_WDT_RST) logger.print("WDT_RST");
    if (resetInfo->reason == REASON_EXCEPTION_RST) logger.print("EXCEPTION_RST");
    if (resetInfo->reason == REASON_SOFT_WDT_RST) logger.print("SOFT_WDT_RST");
    if (resetInfo->reason == REASON_SOFT_RESTART) logger.print("SOFT_RESTART");
    if (resetInfo->reason == REASON_DEEP_SLEEP_AWAKE) logger.print("DEEP_SLEEP_AWAKE");
    if (resetInfo->reason == REASON_EXT_SYS_RST) logger.print("EXT_SYS_RST");
    logger.end();

    setupOta();

    // Setup delta
    lastPoll = 0;

    delta.onMsgSent(handleMsgSent);
    delta.onMsgEvent(handleMsgEvent);
}

void loop() {
  ArduinoOTA.handle();
  invThink();
  sechoThink();
}