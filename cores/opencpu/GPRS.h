#include <Arduino.h>

#define DEBUG_GPRS(F, ...) DBG("[GPRS] " F "\n", ##__VA_ARGS__)

class GPRS
{
protected:
  u8 id;
  ST_PDPContxt_Callback st_callbacks;
  ST_GprsConfig st_apn;
  static void onDeactive(u8 id, s32 ret, void *gprs);
  static void onActive(u8 id, s32 ret, void *gprs);
  bool result;
  u32 event;

public:
  GPRS(u8 context)
  {
    id = context;
    result = -1;
    st_callbacks.Callback_GPRS_Actived = onActive;
    st_callbacks.CallBack_GPRS_Deactived = onDeactive;
    Ql_snprintf((char *)st_apn.apnName, sizeof(st_apn.apnName), "name");
    Ql_snprintf((char *)st_apn.apnUserId, sizeof(st_apn.apnUserId), "user");
    Ql_snprintf((char *)st_apn.apnUserId, sizeof(st_apn.apnUserId), "pass");
    event = Ql_OS_CreateEvent((char *)"GPRS_EVENT");
    Ql_GPRS_Config(id, &st_apn);
    Ql_GPRS_Register(context, &st_callbacks, this);
  };
  bool begin();
  bool begin(const char *apn);
  bool begin(const char *apn, const char *name, const char *pass);
  bool act(bool blocked = true); // blocking mode
  bool deact(void);
  bool acted;
};

extern GPRS gprs;
