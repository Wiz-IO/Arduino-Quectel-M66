/*
 * api.h M66
 *
 *  Created on: 02.03.2019
 *      Author: Georgi Angelov
 */

#ifndef API_H_
#define API_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

    /* portable character for multichar character set */
    typedef char kal_char;
    /* portable wide character for unicode character set */
    typedef unsigned short kal_wchar;
    /* portable 8-bit unsigned integer */
    typedef unsigned char kal_uint8;
    /* portable 8-bit signed integer */
    typedef signed char kal_int8;
    /* portable 16-bit unsigned integer */
    typedef unsigned short int kal_uint16;
    /* portable 16-bit signed integer */
    typedef signed short int kal_int16;
    /* portable 32-bit unsigned integer */
    typedef unsigned int kal_uint32;
    /* portable 32-bit signed integer */
    typedef signed int kal_int32;
    /* portable 64-bit unsigned integer */
    typedef unsigned long long kal_uint64;
    /* portable 64-bit signed integer */
    typedef signed long long kal_int64;
    /* boolean representation */
    typedef enum
    {
        KAL_FALSE, /* FALSE value */
        KAL_TRUE   /* TRUE value */
    } kal_bool;
    typedef unsigned short WCHAR;

#ifndef NULL
#define NULL 0
#endif

    int api_strcmp(const char *s1, const char *s2);
    int api_check_api(void);
    const char *api_getVersion(void);
    int api_messages_available(void);

    int wiz__vsnprintf(char *s, unsigned int n, const char *f, va_list a);
    int api_vsnprintf(char *s, unsigned int n, const char *f, va_list a);
    long api_strtol(const char *nptr, char **endptr, int base);
    unsigned long int api_strtoul(const char *str, char **endptr, int base); // todo

    int api_atoi(const char *str);
    long int api_atol(const char *str);

    unsigned int api_getRandomSeed(void);
    unsigned int api_rand();
    unsigned int api_srand(unsigned int r);

    unsigned int waitMs(unsigned int ms);
    unsigned int api_getMicro(void);
    void api_delayMicro(unsigned int us);

    int api_getIMEI(char *imei);
    int api_getIMSI(char *imsi);
    int api_getSimStatus(void);
    int api_getNetworkState(unsigned char type) /* 1 or 2 */;
    int api_getSignalLevel(void);
    int api_isCFUN(unsigned char cfun_state);

    typedef enum
    {
        SOC_OOBINLINE = 0x01 << 0,        /* not support yet */
        SOC_LINGER = 0x01 << 1,           /* linger on close */
        SOC_NBIO = 0x01 << 2,             /* Nonblocking */
        SOC_ASYNC = 0x01 << 3,            /* Asynchronous notification */
        SOC_NODELAY = 0x01 << 4,          /* disable Nagle algorithm or not */
        SOC_KEEPALIVE = 0x01 << 5,        /* enable/disable the keepalive */
        SOC_RCVBUF = 0x01 << 6,           /* set the socket receive buffer size */
        SOC_SENDBUF = 0x01 << 7,          /* set the socket send buffer size */
        SOC_NREAD = 0x01 << 8,            /* no. of bytes for read, only for soc_getsockopt */
        SOC_PKT_SIZE = 0x01 << 9,         /* datagram max packet size */
        SOC_SILENT_LISTEN = 0x01 << 10,   /* SOC_SOCK_SMS property */
        SOC_QOS = 0x01 << 11,             /* set the socket qos */
        SOC_TCP_MAXSEG = 0x01 << 12,      /* set the max segmemnt size */
        SOC_IP_TTL = 0x01 << 13,          /* set the IP TTL value */
        SOC_LISTEN_BEARER = 0x01 << 14,   /* enable listen bearer */
        SOC_UDP_ANY_FPORT = 0x01 << 15,   /* enable UDP any foreign port */
        SOC_WIFI_NOWAKEUP = 0x01 << 16,   /* send packet in power saving mode */
        SOC_UDP_NEED_ICMP = 0x01 << 17,   /* deliver NOTIFY(close) for ICMP error */
        SOC_IP_HDRINCL = 0x01 << 18,      /* IP header included for raw sockets */
        SOC_IPSEC_POLICY = 0x01 << 19,    /* ip security policy */
        SOC_TCP_ACKED_DATA = 0x01 << 20,  /* TCPIP acked data */
        SOC_TCP_DELAYED_ACK = 0x01 << 21, /* TCP delayed ack */
        SOC_TCP_SACK = 0x01 << 22,        /* TCP selective ack */
        SOC_TCP_TIME_STAMP = 0x01 << 23,  /* TCP time stamp */
        SOC_TCP_ACK_MSEG = 0x01 << 24     /* TCP ACK multiple segment */
    } soc_option_enum;

    int api_soc_getsockopt(unsigned char s, unsigned int option, void *val, unsigned char size);
    int api_soc_setsockopt(unsigned char s, unsigned int option, void *val, unsigned char size);
    unsigned int api_soc_htonl(unsigned int a);
    unsigned short api_soc_htons(unsigned short a);

    int api_base64Decode(const char *src, int srcl, char *dst, int dstl);
    int api_base64Encode(const char *src, int srcl, char *dst, int dstl, bool auto_line_wrap);

    // TODO
    int api_SaveAndSetIRQMask(void) __attribute__((weak));
    void api_RestoreIRQMask(int) __attribute__((weak));

#ifdef __cplusplus
}
#endif

#endif /* API_H_ */