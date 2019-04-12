/*
 *  Created on: 01.15.2019
 *      Author: Georgi Angelov
 */

#include <variant.h>

#define PWM_MAX 8193

static u32 adc_value = 0;

void analogReference(uint8_t mode) {}

void analogWrite(uint8_t pin, int val)
{
  if (PWM0 == pin)
    Ql_PWM_Output((Enum_PinName)pin, val);
}

void analogClose(uint8_t pin)
{
  switch (pin)
  {
  case ADC0:
    Ql_ADC_Sampling((Enum_ADCPin)(pin - PINNAME_END), false);
    adc_value = 0;
    break;
  case PWM0:
    Ql_PWM_Uninit((Enum_PinName)pin);
  }
}

int analogRead(uint8_t pin)
{
  return adc_value; // 0 ~ 2800mV
}

static void onADC(Enum_ADCPin adcPin, u32 adcValue, void *customParam)
{
  adc_value = adcValue;
}

void analogOpen(uint8_t pin, /* val, src, div */...)
{
  switch (pin)
  {
  case ADC0:
  {
    Enum_ADCPin aPin = (Enum_ADCPin)(pin - PINNAME_END);
    Ql_ADC_Register(aPin, onADC, NULL);
    Ql_ADC_Init(aPin, 5, 200);
    Ql_ADC_Sampling(aPin, true);
  }
  break;

  case PWM0:
  {
    va_list list;
    va_start(list, pin);
    uint32_t val = va_arg(list, uint32_t);
    uint32_t pwmSrcClk = va_arg(list, uint32_t);
    uint32_t pwmDiv = va_arg(list, uint32_t);
    Ql_GPIO_Uninit((Enum_PinName)pin);
    uint32_t PWM_lowPulseNum = PWM_MAX / 2;
    uint32_t PWM_highPulseNum = PWM_MAX / 2;
    Ql_PWM_Init((Enum_PinName)pin, (Enum_PwmSource)pwmSrcClk, (Enum_PwmSourceDiv)pwmDiv, PWM_lowPulseNum, PWM_highPulseNum);
    Ql_PWM_Output((Enum_PinName)pin, val);
  }
  break;
  
  } //switch
}
