/*
 *  Udp.cpp: Library to send/receive UDP packets.
 *
 */

#ifndef _GPRS_UDP_H_
#define _GPRS_UDP_H_

#include "Udp.h"
#include "gprsDNS.h"

#define DEBUG_UDP(F, ...) DBG("[UDP] " F "\n", ##__VA_ARGS__)

#define UDP_BUFFER_SIZE (1460)

class gprsUDP : public UDP
{
private:
  int socket;
  IPAddress multicast_ip;
  IPAddress remote_ip;
  uint16_t server_port;
  uint16_t remote_port;
  char *tx_buffer;
  size_t tx_buffer_len;
  cbuf *rx_buffer;
  int id;

public:
  gprsUDP();
  gprsUDP(int context);
  ~gprsUDP();
  uint8_t begin(IPAddress a, uint16_t p);
  uint8_t begin(uint16_t p);
  void stop();
  int beginPacket();
  int beginPacket(IPAddress ip, uint16_t port);
  int beginPacket(const char *host, uint16_t port);
  int endPacket();
  size_t write(uint8_t);
  size_t write(const uint8_t *buffer, size_t size);
  int parsePacket();
  int available();
  int read();
  int read(unsigned char *buffer, size_t len);
  int read(char *buffer, size_t len);
  int peek();
  void flush();
  IPAddress remoteIP();
  uint16_t remotePort();
};

#endif /* _GPRS_UDP_H_ */
