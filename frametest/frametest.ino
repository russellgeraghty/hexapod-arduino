#include <Modbus.h>

#define LED_PIN 13

void setup() {
  Serial.begin(9600);

  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  int count = 0;
  char buffy[13] = {'\0'};
  bool reading = false;
  
  while (count < 14) {
    if (Serial.available()) {
      char c = Serial.read();
      if (':' == c) {
        count = 0;
        Serial.println("Ready to read message");
        reading = true;
      } else if (' ' == c || '\r' == c || '\n' == c) {
        reading = false;
      } else {
        if (reading) {
          buffy[count] = c; 
          count++;
        }       
      }
    }
  }
  
  ModbusMaster master;\
  MasterModbusMessage message;
  master.fromWireFormat(&message, buffy);
  
  Serial.println(message.slave);
  Serial.println(message.function);
}
