/*  M66
 *  Created on: 01.01.2019
 *      Author: Georgi Angelov
 */

#include <interface.h>
#include <DEV.h>
extern DeviceClass Dev;

extern void initVariant() __attribute__((weak));
extern void setup();
extern void loop();

static struct
{
    uint32_t wait;
    uint32_t event;
    ST_MSG msg;
} arduino = {10 /* default task wait */, 0, {0, 0, 0, 0}};

void arduinoSetWait(u32 wait)
{
    arduino.wait = wait == 0 ? 1 : wait;
}

static inline bool arduinoInit(void)
{
    if (0 == arduino.event)
        arduino.event = Ql_OS_CreateEvent((char *)"AEVENT"); // once
    return arduino.event == 0;
}

static inline void arduinoDispatchMessages(void)
{
    switch (arduino.msg.message)
    {
    case MSG_ID_URC_INDICATION:
        Dev.m_Urc(arduino.msg.param1, arduino.msg.param2);
        break;
    default:
        Dev.m_Message(&arduino.msg);
        break;
    }
}

void arduinoProcessMessages(unsigned int wait)
{
    u32 id = Ql_OS_GetActiveTaskId();
    if (ARDUINO_TASK_ID == id)
    {
        Ql_OS_GetMessage(&arduino.msg);                    // there is always more than zero
        Ql_OS_SendMessage(id, MSG_PROCESS_MESSAGES, 0, 0); // send one message
        arduinoDispatchMessages();
    }
    Ql_Sleep(wait);
}

void delayEx(unsigned int ms)
{
#define BLOCK_TIME 100
    unsigned int count = ms / BLOCK_TIME;
    while (count--)
        arduinoProcessMessages(BLOCK_TIME);  // step
    arduinoProcessMessages(ms % BLOCK_TIME); // remain
}

/// Arduino Task
extern "C" void proc_arduino(int id)
{
    TRACE("[A] proc_arduino\n");
    while (arduino.event == 0)
        Ql_Sleep(10);
    Ql_OS_WaitEvent(arduino.event, EVENT_FLAG0);       // wait ril ready
    initVariant();
    Ql_OS_SendMessage(id, MSG_PROCESS_MESSAGES, 0, 0); // dont touch
    TRACE("[A] BEGIN\n");
    arduinoProcessMessages(arduino.wait);
    setup();
    while (true)
    {
        arduinoProcessMessages(arduino.wait);
        loop();
    }
}

/// Main Task
extern "C" void proc_main_task(int taskId)
{
    TRACE("[M] proc_main_task\n");
    if (api_check_api())
    {
        TRACE("[M] ERROR Firmware not support\n");
        abort();
    }
    TRACE("[M] proc_main_task\n");
    if (arduinoInit())
    {
        TRACE("[M] ERROR arduinoInit()\n");
        abort();
    }
    __libc_init_array();
    entry_main(taskId); // if exist - OpenCPU style
    ST_MSG m;
    while (true)
    {
        Ql_OS_GetMessage(&m);
        switch (m.message)
        {
        case MSG_ID_RIL_READY:
            Ql_RIL_Initialize();
            Ql_OS_SetEvent(arduino.event, EVENT_FLAG0); // start arduino
            TRACE("[M] RIL READY\n");
            break;
        case MSG_ID_URC_INDICATION:
            if (m.message > URC_GPRS_NW_STATE_IND)                                 // ignore first urc-s
                Ql_OS_SendMessage(ARDUINO_TASK_ID, m.message, m.param1, m.param2); // resend to arduino task
            break;
        default:
            Ql_OS_SendMessage(ARDUINO_TASK_ID, m.message, m.param1, m.param2); // resend to arduino task
            break;
        } // SWITCH
    }     // WHILE
}

extern "C" void proc_reserved1(int);
extern "C" void proc_reserved2(int);
///////////////////////////////////////////////////////////////////////////////////////////////////////////
