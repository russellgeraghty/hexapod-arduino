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
	boolean seenStart = false;
	boolean seenEnd = false;
	
	int size = strlen(buffer);
	
	if (17 == size) {
		seenStart = ':' == buffer[0];
		buffer++;
		
		int wireChecksum = -1;
		int pos = 0;
		while (*buffer != '\0') {	
			char string_2l[3] = {'\0'};
			strncpy(string_2l, buffer, 2);

			int i = strtol(string_2l, NULL, 16);
			
			switch (pos) {
				case 0:
					message.slave = i;
					break;
				case 1:
					message.function = i;
					break;
				case 2: 
					message.data1 = i;
					break;
				case 3: 
					message.data2 = i;
					break;
				case 4: 
					message.data3 = i;
					break;
				case 5: 
					message.data4 = i;
					break;
				case 6:
					wireChecksum = i;
					break;
				default:
					// Hmm
					break;
			}

			pos++;
			buffer += 2;
		}
		
		int theChecksum = calculateChecksum(buffer, 14);
		if (theChecksum != wireChecksum) {
			// Read modbus spec about what this should be set to...
			message.slave = -1;
		}
	}
	
}

int ModbusMaster::calculateChecksum(char* buffer, int bytes) {
	int lrc = 0;
	while (bytes --)
		lrc += *buffer++;
		
	return ((unsigned char)(-((char) lrc))) ;	
}
