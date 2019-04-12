#ifndef _RILCLASS_H
#define _RILCLASS_H

#include <interface.h>
#include <HardwareSerial.h>
extern HardwareSerial Virtual1;

class RilClass
{
public:
  RilClass(HardwareSerial &uart);

  void begin()
  {
    _uart->end();
    _uart->begin(0);
  }
  void end() { _uart->end(); }

  int noop();
  int single(const char *command);

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

  int SMS_formatText(bool f);
  int SMS_characterSet(const char *cs);
  int SMS_begin(const char *to);
  int SMS_end();

private:
  HardwareSerial *_uart;
  enum
  {
    AT_COMMAND_IDLE,
    AT_RECEIVING_RESPONSE
  } _atCommandState;
  int _ready;
  String _buffer;
  String *_responseDataStorage;
};

extern RilClass Ril;

#endif //_RIL_CLASS_H