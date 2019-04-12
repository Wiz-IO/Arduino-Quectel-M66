#ifndef __DEV_H__
#define __DEV_H__

#include <interface.h>
#include "RilClass.h"
extern HardwareSerial Virtual;

typedef void (*vCallback)(void);
typedef void (*uCallback)(u32, u32);
typedef void (*mCallback)(ST_MSG *msg);

class DeviceClass
{
  public:
    DeviceClass()
    {
        onMessage = NULL;
        onUrc = NULL;
        wtdID = -1;
    }

    RilClass ril = RilClass(Virtual);
    RilClass &operator=(RilClass &ril);
    void begin()
    {
        ril.begin();
    }

    mCallback onMessage;
    void m_Message(ST_MSG *m)
    {
        if (onMessage)
            onMessage(m);
    }

    uCallback onUrc;
    void m_Urc(u32 urc, u32 data)
    {
        if (onUrc)
            onUrc(urc, data);
    }

    const char *getVersion() { return api_getVersion(); }
    void getImsi(char *imsi, uint32_t size = 16);
    void getImsi(String &s);

    void getImei(char *imei, uint32_t size = 16);
    void getImei(String &s);

    int getSimStatus();
    void waitSimReady(const char *pin = NULL);
    void enterPin(const char *pin);

    int getCreg() { return api_getNetworkState(1); }
    void waitCreg();

    int getCgreg() { return api_getNetworkState(2); }
    void waitCgreg();
    void waitCereg();

    void reset() { Ql_Reset(0); }
    void powerOff() { Ql_PowerDown(1); }
    int powerReason() { return Ql_GetPowerOnReason(); }
    void Sleep() { Ql_SleepEnable(); }
    void noSleep() { Ql_SleepDisable(); }

    ///for pin pulse
    int watchdog(uint8_t pin, unsigned int interval = 1000)
    {
        if (interval < 200)
            interval == 200;
        //A2Q(pin);
        return Ql_WTD_Init(0, (Enum_PinName)pin, interval);
    }
    ///internal watchdog
    void beginWatchdog(unsigned int interval = 1000)
    {
        if (interval < 400)
            interval = 400;
        wtdID = Ql_WTD_Start(interval);
    }
    void endWatchdog()
    {
        if (wtdID > 0)
            Ql_WTD_Stop(wtdID);
    }
    void kickWatchdog()
    {
        if (wtdID > 0)
            Ql_WTD_Feed(wtdID);
    }

  private:
    int wtdID;
};

extern DeviceClass Dev;

#endif //__DEV_H__