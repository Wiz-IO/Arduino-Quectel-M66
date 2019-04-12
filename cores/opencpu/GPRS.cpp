#include <GPRS.h>

void GPRS::onActive(u8 id, s32 ret, void *gprs)
{
    DEBUG_GPRS("onActive( %d %d )", id, ret);
    if (gprs)
    {
        GPRS *p = (GPRS *)gprs;
        p->result = (ret == GPRS_PDP_ALREADY || ret == GPRS_PDP_SUCCESS);
        Ql_OS_SetEvent(p->event, 1);
    }
}

void GPRS::onDeactive(u8 id, s32 ret, void *user)
{
    DEBUG_GPRS("onDeactivedGPRS( %d %d )", id, ret);
}

bool GPRS::act(bool blocked)
{
    int r = Ql_GPRS_ActivateEx(id, blocked); // 150 sec
    if (GPRS_PDP_SUCCESS == r || GPRS_PDP_SUCCESS == r)
        return true;
    DEBUG_GPRS("Ql_GPRS_ActivateEx( %d ) = %d", id, r);
    //todo non-blocked
    Ql_OS_WaitEvent(event, 1);
    return result;
}

bool GPRS::deact(void)
{
    int r = Ql_GPRS_Deactivate(id);
    if (GPRS_PDP_SUCCESS == r || GPRS_PDP_SUCCESS == r)
        return true;
    DEBUG_GPRS("Ql_GPRS_Deactivate( %d ) = %d", id, r); // 2 = GPRS_PDP_WOULDBLOCK
    //todo non-blocked
    Ql_OS_WaitEvent(event, 1);
    return result;
}

bool GPRS::begin(const char *name, const char *user, const char *pass)
{
    if (name)
        Ql_snprintf((char *)st_apn.apnName, sizeof(st_apn.apnName), name);
    else
        Ql_snprintf((char *)st_apn.apnName, sizeof(st_apn.apnName), "name");

    if (user)
        Ql_snprintf((char *)st_apn.apnUserId, sizeof(st_apn.apnUserId), user);
    else
        Ql_snprintf((char *)st_apn.apnUserId, sizeof(st_apn.apnUserId), "user");

    if (pass)
        Ql_snprintf((char *)st_apn.apnUserId, sizeof(st_apn.apnUserId), pass);
    else
        Ql_snprintf((char *)st_apn.apnUserId, sizeof(st_apn.apnUserId), "pass");
    if (0 == event)
        event = Ql_OS_CreateEvent((char *)"GPRS_EVENT");
    int res = Ql_GPRS_Config(id, &st_apn);
    return (res == GPRS_PDP_ALREADY || res == GPRS_PDP_SUCCESS);
}
bool GPRS::begin()
{
    return begin(NULL, NULL, NULL);
}

bool GPRS::begin(const char *name)
{
    return begin(name, NULL, NULL);
}

GPRS gprs(0);