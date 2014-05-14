#include <NetFrame.h>

MessageBuilder builder;

void setup() {
  Serial.begin(9600);
  Frame frame;
  frame.source = 1;
  frame.messageType = 6;
  frame.destination = 2;
  frame.userFunction = 'D';
  frame.sign = 0x20;
  frame.functionCode = "BBB";
  frame.payload = "1234";
  
  byte* msg = new byte[15];
  
  builder.toMessageArray(msg, frame);
  
  for (int i = 0; i < 15; i++) {
    Serial.print(msg[i]);
    Serial.print("\n");
  }
}

void loop() {

}
