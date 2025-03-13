/*
    EspLogUdp
    Utility class to send logging messages via UDP.
    By David McDonald 2024
*/

#include "EspLogUdp.h"
#ifdef DEBUG
#include "HardwareSerial.h"
#endif

EspLogUdp::EspLogUdp()
{
    _ipaddr = IPAddress(255, 255, 255, 255); // Default IP is "broadcast"
    _port = 12345;                           // Default port
    _prefix = 0;
    _prefixLength = 0;
}

void EspLogUdp::init(IPAddress addr, uint16_t port)
{
    _ipaddr = addr;
    _port = port;
}

void EspLogUdp::setPrefix(const char *prefix, uint8_t prefixLength)
{
    _prefix = prefix;
    _prefixLength = prefixLength;
}

void EspLogUdp::start()
{
#ifdef DEBUG
    Serial.write(_prefix, _prefixLength);
#endif
    _udp.beginPacket(_ipaddr, _port);
    _udp.write(_prefix, _prefixLength);
    _msgbuilding = true;
}

void EspLogUdp::end()
{
#ifdef DEBUG
    Serial.println();
#endif
    _udp.write('\n');
    _udp.endPacket();
    _msgbuilding = false;
}

size_t  EspLogUdp::write(uint8_t c) {
    return _udp.write(c);
}

size_t  EspLogUdp::write(const uint8_t *buffer, size_t size) {
    return _udp.write(buffer, size);
}

void EspLogUdp::send(uint8_t c)
{
    send(&c, 1);
}

void EspLogUdp::send(const char *cstring)
{
    send(cstring, strlen(cstring));
}

void EspLogUdp::send(const char *buffer, size_t size)
{
    send((uint8_t *)buffer, strlen(buffer));
}

void EspLogUdp::send(uint8_t *buffer, size_t size)
{
    start();
    write(buffer, size);
    end();
}