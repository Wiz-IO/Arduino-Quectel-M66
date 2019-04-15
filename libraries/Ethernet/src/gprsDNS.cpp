/*
 * DNSClient
 *
 *  Created on: 03.03.2019
 *      Author: Georgi Angelov
 */

#include "gprsDNS.h"

static int dns_Code;
static IPAddress dns_IP;
static void Callback_GetIpByName(u8 contexId, u8 requestId, s32 errCode, u32 ipAddrCnt, u32 *ipAddr)
{
     DEBUG_DNS("CALLBACK = %d, %d", errCode, ipAddrCnt);
     dns_Code = errCode;
     if (errCode == SOC_SUCCESS && ipAddrCnt > 0)
     {
          dns_IP = (uint32_t)*ipAddr;
     }
     else
     {
          DEBUG_DNS("[DNS] ERROR Callback: %d\n", errCode);
     }
}

bool DNSClient::getHostByName(const char *host, IPAddress &IP, unsigned char id)
{
     uint32_t ip;
     if (Ql_inet_aton(host, &ip))
     {
          IP = (uint32_t)ip;
          return true;
     }

     IP = (uint32_t)0;
     if (!host || id > 1)
          return false;
     // todo MUTEX_LOCK
     dns_Code = -1;
     int r = Ql_IpHelper_GetIPByHostName(0, 0, (u8 *)host, Callback_GetIpByName);
     // todo SET TIMEOUT
     for (int i = 0; i < 1000; i++)
     {
          arduinoProcessMessages(50);
          if (0 == dns_Code)
               goto ready;
     }
     DEBUG_DNS("[ERROR] DNS TIMEOUT");
     // todo MUTEX_UNLOCK
     return false;
ready:
     DEBUG_DNS("[DNS] READY");
     IP = dns_IP;
     // todo MUTEX_UNLOCK
     return true;
}
