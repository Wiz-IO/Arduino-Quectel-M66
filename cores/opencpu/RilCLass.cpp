
#include "RilClass.h"

#define RIL_MIN_RESPONSE_OR_URC_WAIT_TIME_MS 20

RilClass::RilClass(HardwareSerial &uart) : _uart(&uart),
                                           _atCommandState(AT_COMMAND_IDLE),
                                           _ready(1),
                                           _responseDataStorage(NULL)
{
    _buffer.reserve(256);
}

int RilClass::noop()
{
    send("AT");
    return (waitForResponse() == 1);
}

int RilClass::single(const char *command)
{
    send(command);
    return (waitForResponse() == 1);
}

size_t RilClass::write(uint8_t c)
{
    return _uart->write(c);
}

size_t RilClass::write(const uint8_t *buf, size_t size)
{
    return _uart->write(buf, size);
}

void RilClass::send(const char *command)
{
    TRACE("[RIL] Send: %s\n", command);
    _uart->clear();
    _uart->println(command);
    _atCommandState = AT_COMMAND_IDLE;
    _ready = 0;
}

void RilClass::sendf(const char *fmt, ...)
{
    char buf[BUFSIZ];
    va_list ap;
    va_start((ap), (fmt));
    vsnprintf(buf, sizeof(buf) - 1, fmt, ap);
    va_end(ap);
    send(buf);
}

int RilClass::waitForResponse(unsigned long timeout, String *responseDataStorage)
{
    _responseDataStorage = responseDataStorage;
    for (unsigned long start = millis(); (millis() - start) < timeout;)
    {        
        int r = ready();
        if (r != 0)
        {
            _responseDataStorage = NULL;
            return r;
        }
    }
    _responseDataStorage = NULL;
    _buffer = "";
    return -1;
}

int RilClass::waitForPrompt(unsigned long timeout)
{
    for (unsigned long start = millis(); (millis() - start) < timeout;)
    {
        ready();
        if (_buffer.endsWith(">"))
            return 1;
    }
    return -1;
}

int RilClass::ready()
{    
    poll();
    return _ready;
}

void RilClass::poll()
{
    arduinoProcessMessages(50);
    while (_uart->available())
    {
        char c = _uart->read();
        _buffer += c;
        TRACE("%c", c);
        switch (_atCommandState)
        {
        case AT_COMMAND_IDLE:
        default:
        {
            if (_buffer.startsWith("AT") && _buffer.endsWith("\r\n"))
            {
                _atCommandState = AT_RECEIVING_RESPONSE;
                _buffer = "";
            }
            else if (_buffer.endsWith("\r\n"))
            {
                _buffer = "";
            }
            break;
        }

        case AT_RECEIVING_RESPONSE:
        {
            if (c == '\n')
            {
                int responseResultIndex = _buffer.lastIndexOf("OK\r\n");
                if (responseResultIndex != -1)
                {
                    _ready = 1;
                }
                else
                {
                    responseResultIndex = _buffer.lastIndexOf("ERROR\r\n");
                    if (responseResultIndex != -1)
                    {
                        _ready = 2;
                    }
                    else
                    {
                        responseResultIndex = _buffer.lastIndexOf("NO CARRIER\r\n");
                        if (responseResultIndex != -1)
                        {
                            _ready = 3;
                        }
                    }
                }
                if (_ready != 0)
                {
                    if (_responseDataStorage != NULL)
                    {
                        _buffer.remove(responseResultIndex);
                        _buffer.trim();
                        *_responseDataStorage = _buffer;
                        _responseDataStorage = NULL;
                    }
                    _atCommandState = AT_COMMAND_IDLE;
                    _buffer = "";
                    return;
                }
            }
            break;
        }
        } //switch
    }     //while
}

void RilClass::setResponseDataStorage(String *responseDataStorage)
{
    _responseDataStorage = responseDataStorage;
}

RilClass Ril(Virtual1);