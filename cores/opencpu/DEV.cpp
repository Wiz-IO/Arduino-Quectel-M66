#include "DEV.h"

void DeviceClass::getImsi(char *buffer, uint32_t size)
{
    if (buffer && size > 15)
    {
        memset(buffer, 0, size);
        api_getIMSI(buffer);
    }
}

void DeviceClass::getImsi(String &s)
{
    char buffer[16];
    memset(buffer, 0, 16);
    api_getIMSI(buffer);
    s = buffer;
}

void DeviceClass::getImei(char *buffer, uint32_t size)
{
    if (buffer && size > 15)
    {
        memset(buffer, 0, size);
        api_getIMEI(buffer);
    }
}

void DeviceClass::getImei(String &s)
{
    char buffer[16];
    memset(buffer, 0, 16);
    api_getIMEI(buffer);
    s = buffer;
}

int DeviceClass::getSimStatus()
{
    return api_getSimStatus(); // 0 = READY,
}

void DeviceClass::enterPin(const char *pin)
{
    int res;
    if (pin)
    {
        Dev.ril.sendf("AP+CPIN =\"%s\"", pin);
        if (ril.waitForResponse() == 1)
            return;
    }
    TRACE("[PIN] ERROR\n");
    while (true)
        Ql_Sleep(1000);
}

void DeviceClass::waitSimReady(const char *pin)
{
    int status;
    do
    {
        delayEx(100);
        status = api_getSimStatus();
        if (status == -666) // TODO status enum
            enterPin(pin);
    } while (0 != status);
}

void DeviceClass::waitCreg()
{
    while (1)
    {
        int res = getCreg();
        if (res == 5 || res == 1)
            break;
        delayEx(100);
    }
}

void DeviceClass::waitCgreg()
{
    while (1)
    {
        int res = getCgreg();
        if (res == 5 || res == 1)
            break;
        delayEx(100);
    }
}

void DeviceClass::waitCereg(){
    waitCreg();
    delay(200); 
    waitCgreg();
    delay(200); 
}

DeviceClass Dev;