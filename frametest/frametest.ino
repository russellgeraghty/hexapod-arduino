#include <Modbus.h>

#define MAX_BUFFER 100

void setup() {
  Serial.begin(9600);
}

void loop() {
  char buffy[MAX_BUFFER] = {'\0'};  
  char c = '\0';
  int count = 0;  
  bool reading = false;
  
  while (count < MAX_BUFFER && '\r' != c) {
    if (Serial.available()) {
      c = Serial.read();
      if (':' == c) {
        reading = true;
        // Go back to the start of the buffer
        count = 0;
      } else if ( (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'))  {
        buffy[count++] = c;
      }        
    }
  }
    
  ModbusMaster master;
  MasterModbusMessage message;
  master.fromWireFormat(&message, buffy);
  
  if (message.failedLrc) {
    // Ignore it then.
  } else {
    char error[6] = {'\0'};
    master.toErrorFormat(error, message, 1);
    Serial.println(error);
  }
}
