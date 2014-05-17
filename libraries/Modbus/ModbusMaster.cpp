#include <Arduino.h>
#include <Modbus.h>

ModbusMaster::ModbusMaster() {
}

void ModbusMaster::toWireFormat(char* buffer, MasterModbusMessage message) {
	char* buffy = new char[12];
	sprintf(buffy, "%.2x%.2x%.2x%.2x%.2x%.2x", message.slave, message.function, message.data1, message.data2, message.data3, message.data4);
	int checksum = calculateChecksum(buffy, 12);
	
	sprintf(buffer, ":%s%.2x\r\n", buffy, checksum);
}

void ModbusMaster::fromWireFormat(MasterModbusMessage message, char* buffer) {
	

}

int ModbusMaster::calculateChecksum(char* buffer, int bytes) {
	int lrc = 0;
	while (bytes --)
		lrc += *buffer++;
		
	return ((unsigned char)(-((char) lrc))) ;	
}
