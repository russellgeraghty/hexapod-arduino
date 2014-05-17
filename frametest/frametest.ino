#include <Modbus.h>

void setup() {
  Serial.begin(9600);

  MasterModbusMessage msg;
  msg.slave = 15;
  msg.function = 17;
  msg.data1 = 1;
  msg.data2 = 7;
  msg.data3 = 256;
  msg.data4 = 3;
  
  ModbusMaster master;
  char* buffy = new char[17];
  master.toWireFormat(buffy, msg);
  
  Serial.write((char *) buffy);
  Serial.write("I'm a new line");
}

void loop() {

}
