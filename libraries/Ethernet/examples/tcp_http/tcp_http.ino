#include <gprsClient.h>
#include <GPRS.h>

gprsClient tcp;
#define HOST "wizio.eu"

void tcp_http()
{
  Serial.println("Test TCP\n");
  int timeout = 0;
  int r = tcp.connect(HOST, 80);
  DBG("connect: %d\n", r);
  if (r == 1)
  {
    char data[] = "GET /iot.php HTTP/1.1\r\nHost:" HOST "\r\n\r\n";
    r = tcp.write(data, strlen(data));
    DBG("sent: %d\n", r);
    delay(100);
    do
    {
      while (tcp.available())
      {
        r = tcp.read(); // read byte
        if (r > 0) {
          Serial.print((char)r); 
          continue;
        } else
          goto end;
      }
      delay(100);
    } while (++timeout < 100);
  end:
    delay(10);
  }
  tcp.stop();
}

void setup() {
  String imei;
  Dev.noSleep();
  Serial.begin(115200);
  Serial.printf("Arduino %s\n", Dev.getVersion());
  Dev.getImei(imei);
  Serial.printf("IMEI %s\n", imei.c_str());
  Dev.waitSimReady();
  Serial.println("Sim Ready");
  Dev.waitCereg();
  delay(200); // must
  Serial.println("Net Ready");
  while (false == gprs.act())
    delay(10000);
  Serial.println("GPRS Ready");  
  tcp_http();
  pinMode(LED, OUTPUT);
}

void loop() {
  static int state = 0;
  static int led = 0;
  if(200 % state == 0)
    digitalWrite(LED, ++led & 1);
  state++;
}


/* RESULT:

Arduino M66FAR01A12BT
IMEI 868325029039365
Sim Ready
Net Ready
GPRS Ready
Test TCP

HTTP/1.1 200 OK
Date: Mon, 15 Apr 2019 07:15:00 GMT
Server: Apache
Upgrade: h2,h2c
Connection: Upgrade
Cache-Control: max-age=0
Expires: Mon, 15 Apr 2019 07:15:00 GMT
Content-Length: 47
Content-Type: text/html

[WIZIO.EU] Hello World ( 2019-04-15 10:15:00 )

*/