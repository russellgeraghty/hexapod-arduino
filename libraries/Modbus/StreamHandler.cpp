#include <Arduino.h>
#include <Modbus.h>
#include <string.h>

StreamHandler::StreamHandler(Stream* _stream) {
	stream = _stream;
}

void StreamHandler::readMessage(char* buffer, int bufferSize) {
	char c = '\0';
	int count = 0;  
	bool reading = false;
  
	while (count < bufferSize && '\r' != c) {
		if (stream->available()) {
			c = stream->read();
			if (':' == c) {
				reading = true;
				// Go back to the start of the buffer
				count = 0;
			} else if (reading && ( (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'))) {
				buffer[count++] = c;
			}        
		}
	}
}

void StreamHandler::writeMessage(char* buffer) {
	stream->println(buffer);	
}