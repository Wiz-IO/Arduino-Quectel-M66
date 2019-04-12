/*
  wiring.c

  Copyright (c) 2019 Georgi Angelov

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
*/

#include <variant.h>

inline void yield(void)
{
  Ql_Sleep(1);
}

inline unsigned int millis()
{
  return Ql_GetMsSincePwrOn();
}

inline unsigned int seconds(void)
{
  return millis() / 1000;
}

inline unsigned int micros()
{
  return api_getMicro();
}

inline void delayMicroseconds(unsigned int us)
{
  api_delayMicro(us);
}

inline void delay(unsigned int ms)
{
  Ql_Sleep(ms);
}

///////////////////////////////////////////////////////////

#define ARRAYLEN(x) (sizeof(x) / sizeof((x)[0]))

PinDescription *getArduinoPin(uint8_t arduinoPin)
{
  for (int i = 0; i < ARRAYLEN(pinsMap); i++)
    if (pinsMap[i].arduino == arduinoPin)
      return &pinsMap[i];
  return NULL;
}

PinDescription *getDevicePin(uint8_t devicePin)
{
  for (int i = 0; i < ARRAYLEN(pinsMap); i++)
    if (pinsMap[i].device == devicePin)
      return &pinsMap[i];
  return NULL;
}

static void eint_callback(Enum_PinName eintPinName, Enum_PinLevel pinLevel, void *user)
{
  Enum_PinName pin = (Enum_PinName)user;
  PinDescription *n = getDevicePin(pin);
  if (n && n->eint)
  {
    Ql_EINT_Mask(pin);
    eint_callback_t cb = (eint_callback_t)n->eint;
    cb(Ql_EINT_GetLevel(n->device));
    Ql_EINT_Unmask(pin);
  }
}

void eintMode(uint8_t pin, Enum_EintType type, eint_callback_t cb, uint32_t hwDebounce, uint32_t swDebounce, bool automask)
{
  PinDescription *n = getArduinoPin(pin);
  if (n)
  {
    if (type == EINT_CLOSE)
    {
      Ql_EINT_Uninit(n->device);
      n->eint = NULL;
    }
    else if (cb)
    {
      n->eint = cb;
      Ql_EINT_RegisterFast(n->device, eint_callback, (void *)((int)n->device));
      Ql_EINT_Init(n->device, type, hwDebounce, swDebounce, automask);
    }
  }
}

///////////////////////////////////////////////////////////

static int _irq_;
int api_SaveAndSetIRQMask(void) __attribute__((weak)); 
void api_RestoreIRQMask(int) __attribute__((weak)); 

void interrupts(void)
{
  api_RestoreIRQMask(_irq_);
}

void noInterrupts(void)
{
  _irq_ = api_SaveAndSetIRQMask();
}