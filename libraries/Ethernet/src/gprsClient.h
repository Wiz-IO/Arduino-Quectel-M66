/*
  gprsClient.cpp - Library
*/

#ifndef _GPRS_TCP_H_
#define _GPRS_TCP_H_

#include <interface.h>
#include "Client.h"
#include "IPAddress.h"
#include "gprsDNS.h"

#define DEBUG_TCP(F, ...)
//DBG("[TCP] " F "\n", ##__VA_ARGS__)

class gprsClient : public Client
{

private:
  char id;
  unsigned char Peek;
  bool peeked;

public:
  gprsClient();
  gprsClient(int contextID);
  gprsClient(int soc, int contextID);
  ~gprsClient();

  int connect(unsigned int ip, unsigned short port);
  virtual int connect(IPAddress ip, unsigned short port);
  virtual int connect(const char *host, unsigned short port);
  virtual size_t write(unsigned char);
  virtual size_t write(const unsigned char *buf, size_t size);
  virtual int available();
  virtual int read();
  virtual int read(unsigned char *buf, size_t size);
  virtual int peek();
  virtual void flush() {}
  virtual void stop();
  virtual unsigned char connected();
  virtual operator bool();

  friend class WiFiServer;
  using Print::write;

private:
  int m_socket;
  bool m_externalSocket;
};

#endif // _GPRS_TCP_H_