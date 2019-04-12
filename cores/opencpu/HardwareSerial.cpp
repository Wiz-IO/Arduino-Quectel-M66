/*
 *  Created on: 01.01.2019
 *      Author: Georgi Angelov
 */

#include <string.h>
#include <inttypes.h>
#include "Arduino.h"
#include <HardwareSerial.h>

#define DEBUG_UART TRACE

extern "C" void debug_enable(unsigned long port);
extern "C" void debug_disable(void);

void HardwareSerial::debug(void)
{
    debug_enable(port);
}

void HardwareSerial::nodebug(void)
{
    debug_disable();
}

/*static*/ void HardwareSerial::callback(Enum_SerialPort port, Enum_UARTEventType event, bool pinLevel, void *serial)
{
  //DBG("[UART] callback port=%d, event=%d\n", port, event);
  if (NULL == serial)
    return;
  if (event == EVENT_UART_READY_TO_READ)
  {
    uint8_t c;
    HardwareSerial *p = (HardwareSerial *)serial;
    while (Ql_UART_Read((Enum_SerialPort)port, &c, 1) > 0)
    {
      //DEBUG_UART("%c", c);
      if (p->save(c))
        return;
    }
  }
}

HardwareSerial::HardwareSerial(uint32_t id)
{
  port = (Enum_SerialPort)id;
  _rx_buffer_head = _rx_buffer_tail = 0;
}

void HardwareSerial::begin(unsigned long baud, void *config) // TODO
{
  _rx_buffer_head = _rx_buffer_tail = 0;
  Ql_memset(_rx_buffer, 0, SERIAL_RX_BUFFER_SIZE);
  int res = Ql_UART_Register(port, (CallBack_UART_Notify)this->callback, this);
  DEBUG_UART("Ql_UART_Register( %d ) = %d\n", port, res);
  if (config)
    res = Ql_UART_OpenEx(port, (ST_UARTDCB *)config);
  else
    res = Ql_UART_Open(port, baud, FC_NONE);
  DEBUG_UART("Ql_UART_Open( %d ) = %d\n", port, res);
}

void HardwareSerial::begin(unsigned long baud)
{
  begin(baud, NULL);
}

void HardwareSerial::end()
{
  Ql_UART_Close(port);
  DEBUG_UART("Ql_UART_Close( %d )\n", port);
  _rx_buffer_head = _rx_buffer_tail;
}

size_t HardwareSerial::write(uint8_t c)
{
  //DEBUG_UART("> %c\n", c);
  return Ql_UART_Write(port, &c, 1);
}

size_t HardwareSerial::write(const uint8_t *buf, size_t size)
{
  if (buf && size)
  {
    //DEBUG_UART("> %s\n", buf);
    return Ql_UART_Write(port, (uint8_t *)buf, size);
  }
  return 0;
}

int HardwareSerial::available(void)
{
  return ((unsigned int)(SERIAL_RX_BUFFER_SIZE + _rx_buffer_head - _rx_buffer_tail)) % SERIAL_RX_BUFFER_SIZE;
}

int HardwareSerial::peek(void)
{
  if (_rx_buffer_head == _rx_buffer_tail)
    return -1;
  else
    return _rx_buffer[_rx_buffer_tail];
}

int HardwareSerial::read(void)
{
  if (_rx_buffer_head == _rx_buffer_tail)
  {
    return -1;
  }
  else
  {
    unsigned char c = _rx_buffer[_rx_buffer_tail];
    _rx_buffer_tail = (buffer_index_t)(_rx_buffer_tail + 1) % SERIAL_RX_BUFFER_SIZE;
    return c;
  }
}

void HardwareSerial::clear(int who)
{
  if (who & 0x10)
  {
    Ql_UART_ClrTxBuffer(port);
  }
  if (who & 0x01)
  {
    Ql_UART_ClrRxBuffer(port);
    _rx_buffer_head = _rx_buffer_tail = 0;
    Ql_memset(_rx_buffer, 0, SERIAL_RX_BUFFER_SIZE);
  }
}

int HardwareSerial::save(uint8_t c)
{
  uint32_t i = (uint32_t)(_rx_buffer_head + 1) % SERIAL_RX_BUFFER_SIZE;
  if (i != _rx_buffer_tail)
  {
    _rx_buffer[_rx_buffer_head] = c;
    _rx_buffer_head = i;
    return 0; // saved
  }
  return 1; // full
}