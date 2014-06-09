#include <Arduino.h>
#include <Modbus.h>
#include <string.h>
#include <Walker.h>

#define RETAIN_CONTROL true

StreamHandler::StreamHandler(TwoWire* _wire) {
	wire = _wire;
}

int StreamHandler::readMessage(int slave, char* buffer, int bufferSize) {
	char c = '\0';
	int count = 0;  
	bool reading = false;
	
	char b[MAX_BUFFER];
	for (int i=0; i<MAX_BUFFER; i++) {
		b[i] = '\0';
	}
	
	wire->requestFrom(slave, bufferSize);
	//int read = wire->readBytesUntil('\r', b, bufferSize); 
	//Wire.requestFrom(2, 6);    // request 6 bytes from slave device #2

	while(wire->available())    // slave may send less than requested
	{ 
		char c = wire->read(); // receive a byte as character
		Serial.print(c);         // print the character
	}

	int read = 0;
	Serial.print("Bytes until ");
	Serial.println(b);
  
	if (read > 0) {
		for (int i=0; i<read && count<bufferSize; i++) {
			c = b[i];
			if (':' == c) {
				reading = true;
				// Go back to the start of the buffer
				count = 0;
			} else if (reading && ( (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'))) {
				buffer[count++] = c;
			} 
		}
	}
	return count;
}

bool StreamHandler::writeMessage(int slave, char* buffer) {
	// Check the leg is there first
	wire->beginTransmission(slave);
	byte response = wire->endTransmission();
	if (response == 0) {
		wire->beginTransmission(slave);
		wire->println(buffer);
		response = wire->endTransmission();
	 } else {
		Serial.print("Slave ");
		Serial.print(slave);
		Serial.print(" not found");
	}
	
	return response == 0;
}

