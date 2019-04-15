/*
 * DNSClient
 *
 *  Created on: 03.03.2019
 *      Author: Georgi Angelov
 */

#ifndef DNSClient_h
#define DNSClient_h

#include <Arduino.h>
#include "IPAddress.h"

#define DEBUG_DNS(F, ...)
//DBG("[DNS] " F "\n", ##__VA_ARGS__)

class DNSClient
{
public:
  bool getHostByName(const char *host, IPAddress &ip, unsigned char id = 0);
};

#endif //DNSClient_h
