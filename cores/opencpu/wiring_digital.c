/*
  wiring_digital.c - digital input and output functions
  Part of Arduino - http://www.arduino.cc/

  Copyright (c) 2005-2006 David A. Mellis

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General
  Public License along with this library; if not, write to the
  Free Software Foundation, Inc., 59 Temple Place, Suite 330,
  Boston, MA  02111-1307  USA

  Modified 28 September 2018 by Georgi Angelov
*/

#include <Arduino.h>
#include <variant.h>

void pinMode(uint8_t pin, uint8_t mode)
{
  PinDescription *n = getArduinoPin(pin);
  if (n)
  {
    if (mode == CLOSE)
    {
      Ql_GPIO_Uninit(n->device);
      n->device = PINNAME_END;
    }
    else
    {
      Enum_PinPullSel pull = PINPULLSEL_DISABLE;
      Enum_PinDirection dir = PINDIRECTION_IN;
      Enum_PinLevel level = PINLEVEL_LOW;
      if (mode & INPUT_PULLUP)
        pull = PINPULLSEL_PULLUP;
      if (mode & INPUT_PULLDOWN)
        pull = PINPULLSEL_PULLDOWN;
      if ((mode & OUTPUT) || (mode & OUTPUT_LO) || (mode & OUTPUT_HI))
        dir = PINDIRECTION_OUT;
      if (mode & OUTPUT_HI)
        level = PINLEVEL_HIGH;
      Ql_GPIO_Init(n->device, dir, level, pull);
    }
  }
}

void digitalWrite(uint8_t pin, uint8_t val)
{
  PinDescription *n = getArduinoPin(pin);
  if (n)
    Ql_GPIO_SetLevel(n->device, (Enum_PinLevel)val & 1);
}

int digitalRead(uint8_t pin)
{
  PinDescription *n = getArduinoPin(pin);
  if (n)
    return Ql_GPIO_GetLevel(n->device);
  return -1;
}
