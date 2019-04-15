/*
  This file is part of the MKR GSM library.
  Copyright (C) 2017  Arduino AG (http://www.arduino.cc/)

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

  MOD for Quectel by Georgi Angelov
*/

#include "Modem.h"

#define MODEM_MIN_RESPONSE_OR_URC_WAIT_TIME_MS 20

ModemClass::ModemClass(HardwareSerial &uart) : _uart(&uart),
                                               _lastResponseOrUrcMillis(0),
                                               _atCommandState(AT_COMMAND_IDLE),
                                               _ready(1),
                                               _responseDataStorage(NULL)
{
    _buffer.reserve(64);
}

void ModemClass::begin(bool restart)
{
    if (restart)
        end();
    _uart->begin(0);
}

void ModemClass::end()
{
    _uart->end();
}

int ModemClass::noop()
{
    send("AT");
    return (waitForResponse() == 1);
}

size_t ModemClass::write(uint8_t c)
{
    return _uart->write(c);
}

size_t ModemClass::write(const uint8_t *buf, size_t size)
{
    return _uart->write(buf, size);
}

void ModemClass::send(const char *command)
{
    TRACE("[MODEM] send: %s\n", command);
    unsigned long delta = millis() - _lastResponseOrUrcMillis;
    if (delta < MODEM_MIN_RESPONSE_OR_URC_WAIT_TIME_MS)
    {
        delay(MODEM_MIN_RESPONSE_OR_URC_WAIT_TIME_MS - delta);
    }
    _uart->println(command);
    _atCommandState = AT_COMMAND_IDLE;
    _ready = 0;
}

void ModemClass::sendf(const char *fmt, ...)
{
    char buf[BUFSIZ];
    va_list ap;
    va_start((ap), (fmt));
    vsnprintf(buf, sizeof(buf) - 1, fmt, ap);
    va_end(ap);
    send(buf);
}

int ModemClass::waitForResponse(unsigned long timeout, String *responseDataStorage)
{
    _responseDataStorage = responseDataStorage;
    for (unsigned long start = millis(); (millis() - start) < timeout;)
    {
        arduinoProcessMessages(50);
        int r = ready();
        if (r != 0)
        {
            _responseDataStorage = NULL;
            return r;
        }
    }
    _responseDataStorage = NULL;
    _buffer = "";
    return -1;
}

int ModemClass::waitForPrompt(unsigned long timeout)
{
    for (unsigned long start = millis(); (millis() - start) < timeout;)
    {
        ready();
        if (_buffer.endsWith(">"))
        {
            return 1;
        }
    }
    return -1;
}

int ModemClass::ready()
{
    poll();
    return _ready;
}

void ModemClass::poll()
{
    while (_uart->available())
    {
        char c = _uart->read();
        _buffer += c;
        switch (_atCommandState)
        {
        case AT_COMMAND_IDLE:
        default:
        {
            if (_buffer.startsWith("AT") && _buffer.endsWith("\r\n"))
            {
                _atCommandState = AT_RECEIVING_RESPONSE;
                _buffer = "";
            }
            else if (_buffer.endsWith("\r\n"))
            {
                _buffer = "";
            }
            break;
        }

        case AT_RECEIVING_RESPONSE:
        {
            if (c == '\n')
            {
                _lastResponseOrUrcMillis = millis();
                int responseResultIndex = _buffer.lastIndexOf("OK\r\n");
                if (responseResultIndex != -1)
                {
                    _ready = 1;
                }
                else
                {
                    responseResultIndex = _buffer.lastIndexOf("ERROR\r\n");
                    if (responseResultIndex != -1)
                    {
                        _ready = 2;
                    }
                    else
                    {
                        responseResultIndex = _buffer.lastIndexOf("NO CARRIER\r\n");
                        if (responseResultIndex != -1)
                        {
                            _ready = 3;
                        }
                    }
                }
                if (_ready != 0)
                {
                    if (_responseDataStorage != NULL)
                    {
                        _buffer.remove(responseResultIndex);
                        _buffer.trim();
                        *_responseDataStorage = _buffer;
                        _responseDataStorage = NULL;
                    }
                    _atCommandState = AT_COMMAND_IDLE;
                    _buffer = "";
                    return;
                }
            }
            break;
        }
        } //switch
    }     //while
}

void ModemClass::setResponseDataStorage(String *responseDataStorage)
{
    _responseDataStorage = responseDataStorage;
}

//ModemClass MODEM(Virtual1);