#include "Arduino.h"
#include <cstring>

#ifndef NetFrame_h
#define NetFrame_h

const int MESSAGE_START = 0x0;
const int MESSAGE_END = 0x3;
const int ACK = 6;
const int NAK = 15;
const int ENQ = 5;

/**
 * A frame object for sending data over RS485.
 */
typedef struct Frame {
	int source;
	int destination;
	int messageType;
	char userFunction;
	String functionCode;
	char sign;
	String payload;
} Frame;

class MessageBuilder {
	public:
		MessageBuilder();
		void toMessageArray(byte* message, Frame frame);
	private:
		void calculateCheckSum(byte* checksum, byte* message);
};

#endif
