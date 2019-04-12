/*
  Quectel M66 - Test
  Created on: 01.04.2019
  Author: Georgi Angelov
*/
    
void setup()
{
  int t = millis();
  String imei;

  Serial.begin(115200);
  Serial.debug(); // enable debug for this port, DBG( work as printf )

  Serial.printf("Arduino      %s\n", Dev.getVersion());
  Dev.getImei(imei);
  Serial.printf("IMEI         %s\n", imei.c_str());

  Dev.waitSimReady();
  Serial.printf("SIM Ready\n");

  Dev.waitCereg(); // wait GREG and CGREG
  delay(200); 
  Serial.printf("NET Ready\n");
  
  pinMode(LED, OUTPUT);
  digitalWrite(LED, 1);
  Serial.printf("Elapsed: %d mili seconds\n", millis() - t);
}

void loop()
{
  static int led = 0;
  delay(200);
  Serial.print(">");
  digitalWrite(LED, ++led & 1);
  /* loop must be state machine, dont make long delays */
}
