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
*/

#include "Modem.h"

#include "SMS.h"

enum
{
  SMS_STATE_IDLE,
  SMS_STATE_LIST_MESSAGES,
  SMS_STATE_WAIT_LIST_MESSAGES_RESPONSE
};

SMS::SMS(bool synch) : _synch(synch),
                       _state(SMS_STATE_IDLE),
                       _smsTxActive(false)
{
}

size_t SMS::write(uint8_t c)
{
  if (_smsTxActive)
  {
    return MODEM.write(c);
  }
  return 0;
}

/*
  0 pdu
  1 text
*/
int SMS::formatText(bool f)
{
  MODEM.sendf("AT+CMGF=%d", (int)f);
  return (MODEM.waitForResponse() == 1);
}

/*
  "GSM" GSM default alphabet
  "HEX" Character strings consist only of hexadecimal numbers from 00 to FF
  "IRA" International reference alphabet
  "PCCP437" PC character set code
  "UCS2" UCS2 alphabet
  "8859-1" ISO 8859 Latin 1 character set
*/
int SMS::characterSet(const char *cs)
{
  MODEM.sendf("AT+CSCS=\"%s\"", cs);
  return (MODEM.waitForResponse() == 1);
}

int SMS::beginSMS(const char *to)
{
  MODEM.sendf("AT+CMGS=\"%s\"", to);
  if (MODEM.waitForResponse(100) == 2)
  {
    _smsTxActive = false;
    return (_synch) ? 0 : 2;
  }
  _smsTxActive = true;
  return 1;
}

int SMS::ready()
{
  int ready = MODEM.ready();

  if (ready == 0)
  {
    return 0;
  }

  switch (_state)
  {
  case SMS_STATE_IDLE:
  default:
  {
    break;
  }

  case SMS_STATE_LIST_MESSAGES:
  {
    MODEM.setResponseDataStorage(&_incomingBuffer);
    MODEM.send("AT+CMGL=\"REC UNREAD\"");
    _state = SMS_STATE_WAIT_LIST_MESSAGES_RESPONSE;
    ready = 0;
    break;
  }

  case SMS_STATE_WAIT_LIST_MESSAGES_RESPONSE:
  {
    _state = SMS_STATE_IDLE;
    break;
  }
  }
  return ready;
}

int SMS::endSMS()
{
  int r;
  if (_smsTxActive)
  {
    MODEM.write(26);
    if (_synch)
    {
      while ((r = MODEM.ready()) == 0)
      {
        delay(100);
      }
    }
    else
    {
      r = MODEM.ready();
    }
    return r;
  }
  else
  {
    return (_synch ? 0 : 2);
  }
}

int SMS::available()
{
  if (_incomingBuffer.length() != 0)
  {
    int nextMessageIndex = _incomingBuffer.indexOf("\r\n+CMGL: ");

    if (nextMessageIndex != -1)
    {
      _incomingBuffer.remove(0, nextMessageIndex + 2);
    }
    else
    {
      _incomingBuffer = "";
    }
  }
  if (_incomingBuffer.length() == 0)
  {
    int r;
    if (_state == SMS_STATE_IDLE)
    {
      _state = SMS_STATE_LIST_MESSAGES;
    }
    if (_synch)
    {
      while ((r = ready()) == 0)
      {
        delay(100);
      }
    }
    else
    {
      r = ready();
    }
    if (r != 1)
    {
      return 0;
    }
  }
  if (_incomingBuffer.startsWith("+CMGL: "))
  {
    _smsDataIndex = _incomingBuffer.indexOf('\n') + 1;
    _smsDataEndIndex = _incomingBuffer.indexOf("\r\n+CMGL: ");
    if (_smsDataEndIndex == -1)
    {
      _smsDataEndIndex = _incomingBuffer.length() - 1;
    }
    return (_smsDataEndIndex - _smsDataIndex) + 1;
  }
  else
  {
    _incomingBuffer = "";
  }
  return 0;
}

int SMS::remoteNumber(char *number, int nlength)
{
#define PHONE_NUMBER_START_SEARCH_PATTERN "\"REC UNREAD\",\""
  int phoneNumberStartIndex = _incomingBuffer.indexOf(PHONE_NUMBER_START_SEARCH_PATTERN);
  if (phoneNumberStartIndex != -1)
  {
    int i = phoneNumberStartIndex + sizeof(PHONE_NUMBER_START_SEARCH_PATTERN) - 1;
    while (i < (int)_incomingBuffer.length() && nlength > 1)
    {
      char c = _incomingBuffer[i];
      if (c == '"')
      {
        break;
      }
      *number++ = c;
      nlength--;
      i++;
    }
    *number = '\0';
    return 1;
  }
  else
  {
    *number = '\0';
  }
  return 2;
}

int SMS::read()
{
  int bufferLength = _incomingBuffer.length();
  if (_smsDataIndex < bufferLength && _smsDataIndex <= _smsDataEndIndex)
  {
    return _incomingBuffer[_smsDataIndex++];
  }
  return -1;
}

int SMS::peek()
{
  if (_smsDataIndex < (int)_incomingBuffer.length() && _smsDataIndex <= _smsDataEndIndex)
  {
    return _incomingBuffer[_smsDataIndex];
  }
  return -1;
}

void SMS::flush()
{
  int smsIndexStart = _incomingBuffer.indexOf(' ');
  int smsIndexEnd = _incomingBuffer.indexOf(',');

  if (smsIndexStart != -1 && smsIndexEnd != -1)
  {
    while (MODEM.ready() == 0)
      ;
    MODEM.sendf("AT+CMGD=%s", _incomingBuffer.substring(smsIndexStart + 1, smsIndexEnd).c_str());
    if (_synch)
    {
      MODEM.waitForResponse(55000);
    }
  }
}
