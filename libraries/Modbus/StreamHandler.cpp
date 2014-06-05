#include <Arduino.h>
#include <Modbus.h>
#include <string.h>
#include <Walker.h>

StreamHandler::StreamHandler(Stream* _stream) {
	stream = _stream;
}

int StreamHandler::readMessage(char* buffer, int bufferSize) {
	char c = '\0';
	int count = 0;  
	bool reading = false;
	
	char b[MAX_BUFFER];
	for (int i=0; i<MAX_BUFFER; i++) {
		b[i] = '\0';
	}
	
	stream->setTimeout(1500);
	int read = stream->readBytesUntil('\r', b, MAX_BUFFER); 
  
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
	return read;
}

void StreamHandler::writeMessage(char* buffer) {
	stream->println(buffer);	
}

