/*
    EspLogUdp
    Utility class to send logging messages via UDP
    By David McDonald 2024

    Supports:
        Sending messages
        Can set destination IP and port
        Supports broadcast 255.255.255.255
        Sending a prefix

    Does not support
        Multicast
        Print lines with or without a prefix. Initial attempt caused packet fragmentation. Maybe look into buffer solution.
        Print class functionality. Due to println() doing 2 prints (one for data, one for \r\n), caused UDP fragmentation.

    Inspired by:

    https://github.com/ericgu/UdpLogger
    Hard coded to broadcast.
    Works, but doesn't extend the "Print" class, can't use function calls that we all know and love.
    Could argue that UpdLogger is more lightweight, could argue that what Print.h includes is already included anyway.
    Turns out that you should extend Print anyway, doesn't work well for network packets.

    https://github.com/mriksman/esp8266-udp-logging
    Is a simple proof of concept, but works as a reference.

    Notes:
    * Works best with whole and complete log messages. Doing multiple calls for 1 line of message risks fragmentation.
    * Don't recommend having multiple instances at once, causes issues on ESP8266, dropped packets.
    * Can't send direct messages using Print functions driven by "write" function, 
      because println does 2 calls to print(), resulting in a split UDP message.
      To use print functions, must use .start(), .print() all of the content, then finish with .end()
*/

#include "WiFiUdp.h"
#include "IPAddress.h"
#include "Print.h"

#ifdef DEBUG
#include <HardwareSerial.h>
#endif

class EspLogUdp : public Print
{
public:
    EspLogUdp();
    // Setup the logger. Default broadcast port 12345
    void init(IPAddress addr, uint16_t port);
    // Set the prefix. Default: (empty)
    void setPrefix(const char *prefix, uint8_t prefixLength);
    void setPrefix(const char *prefix) { setPrefix(prefix, strlen(prefix)); };
    void send(uint8_t *buffer, size_t size);
    void send(const char *buffer, size_t size);
    void send(const char *cstring);
    void send(uint8_t c);
    void start();
    void end();

    // Write a single byte into the packet
    size_t write(uint8_t) override;
    // Write size bytes from buffer into the packet
    size_t write(const uint8_t *buffer, size_t size) override;

private:
    IPAddress _ipaddr;
    uint16_t _port;
    WiFiUDP _udp;
    const char *_prefix;
    uint8_t _prefixLength;
    bool _msgbuilding;
};