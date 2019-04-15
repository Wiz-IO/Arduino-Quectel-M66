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

  https://github.com/arduino-libraries/MKRGSM/tree/master/src
  
  MOD for Quectel by Georgi Angelov
*/

#ifndef _MODEM_INCLUDED_H
#define _MODEM_INCLUDED_H

#include <Arduino.h>

class ModemClass
{
public:
  ModemClass(HardwareSerial &uart);

  void begin(bool restart = false);
  void end();

  int noop(); // AT OK

  size_t write(uint8_t c);
  size_t write(const uint8_t *, size_t);

  void send(const char *command);
  void send(const String &command) { send(command.c_str()); }
  void sendf(const char *fmt, ...);

  int waitForResponse(unsigned long timeout = 500, String *responseDataStorage = NULL);
  int waitForPrompt(unsigned long timeout = 500);
  int ready();
  void poll();
  void setResponseDataStorage(String *responseDataStorage);

private:
  HardwareSerial *_uart;
  unsigned long _lastResponseOrUrcMillis;

  enum
  {
    AT_COMMAND_IDLE,
    AT_RECEIVING_RESPONSE
  } _atCommandState;

  int _ready;
  String _buffer;
  String *_responseDataStorage;
};

//extern ModemClass MODEM;

#endif