/*
    gprsClient.cpp - Library

    Created on: 01.03.2019
        Author: Georgi Angelov
 */

#include "gprsClient.h"

gprsClient::gprsClient()
{
    id = 0;
    peeked = 0;
    Peek = 0;
    m_externalSocket = true;
    m_socket = -1;
}

gprsClient::gprsClient(int contextID)

{
    id = contextID;
    peeked = 0;
    Peek = 0;
    m_externalSocket = 0;
    m_socket = -1;
}

gprsClient::gprsClient(int socket, int contextID)
{
    id = contextID;
    peeked = 0;
    Peek = 0;
    m_externalSocket = 1;
    m_socket = socket;
}

gprsClient::~gprsClient()
{
    if (!m_externalSocket)
        stop();
}

int gprsClient::connect(unsigned int ip, unsigned short port)
{
    if (m_socket)
        stop();
    m_socket = Ql_SOC_Create(id, SOC_TYPE_TCP);
    m_externalSocket = true;
    if (m_socket < 0)
    {
        DEBUG_TCP("Ql_SOC_Create() failed!");
        return false;
    }
    int res = Ql_SOC_ConnectEx(m_socket, (unsigned int)Ql_convertIP(ip), port, true); // blocked
    DEBUG_TCP("Ql_SOC_Connect( soc = %d ) r = %d", m_socket, res);
    return res == SOC_SUCCESS; // true = OK
}

int gprsClient::connect(IPAddress IP, unsigned short port)
{
    return connect((uint32_t)IP, port);
}

int gprsClient::connect(const char *host, unsigned short port)
{
    uint32_t ip;
    IPAddress IP;
    DNSClient d;
    if (d.getHostByName(host, IP))
        return connect(IP, port);
    DEBUG_TCP("could not get host from dns");
    return false;
}

size_t gprsClient::write(unsigned char b)
{
    if (m_socket == -1)
        return 0;
    int w = Ql_SOC_Send(m_socket, &b, 1);
    DEBUG_TCP("Ql_SOC_Send( 1 ) w = %d", w);
    return w > -1 ? 1 : 0;
}

size_t gprsClient::write(const unsigned char *buffer, size_t size)
{
    if (m_socket == -1 || NULL == buffer || size == 0)
        return 0;
    int w = Ql_SOC_Send(m_socket, (u8 *)buffer, size);
    DEBUG_TCP("Ql_SOC_Send( %d ) w = %d", size, w);
    return w > -1 ? w : 0;
}

int gprsClient::read()
{
    if (m_socket == -1)
        return -1;
    if (peeked)
    {
        peeked = false;
        return Peek;
    }
    unsigned char b;
    return (1 == Ql_SOC_Recv(m_socket, &b, 1)) ? b : -1; // no wait
}

int gprsClient::read(unsigned char *buffer, size_t size)
{
    if (m_socket == -1 || NULL == buffer || size == 0)
        return 0;
    unsigned char *p = buffer;
    if (peeked)
    {
        peeked = false;
        *p++ = Peek;
        size -= 1;
        if (0 == size)
            return 1;
    }
    int r = Ql_SOC_Recv(m_socket, p, size); // no wait
    DEBUG_TCP("Ql_SOC_Recv( %d ) r = %d", size, r);
    return r > -1 ? r : -1;
}

int gprsClient::available()
{
    //return m_socket > -1 && peek() > -1;// without soc_getsockopt
    short val;
    return (0 == api_soc_getsockopt(m_socket, SOC_NREAD, &val, 2)) ? val : -1;
}

int gprsClient::peek()
{
    if (m_socket > -1)
    {
        if (peeked)
        {
            DEBUG_TCP("peeked( %02X )", (int)Peek & 0xFF);
            return Peek;
        }
        peeked = 1 == Ql_SOC_Recv(m_socket, &Peek, 1); // no wait
        if (peeked)
        {
            DEBUG_TCP("peek( %02X )", (int)Peek & 0xFF);
            return Peek;
        }
    }
    return -1;
}

void gprsClient::stop()
{
    if (m_socket > -1)
    {
        Ql_SOC_Close(m_socket);
        DEBUG_TCP("Ql_SOC_Close()");
        m_socket = -1;
    }
}

unsigned char gprsClient::connected()
{
    return m_socket > -1;
}

gprsClient::operator bool()
{
    return connected();
}