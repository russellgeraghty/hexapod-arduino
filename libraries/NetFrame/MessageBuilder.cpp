#include <Arduino.h>
#include <NetFrame.h>

const byte BLANK_FIELD = 0x20;

MessageBuilder::MessageBuilder() {
}

/**
 * Convert a frame into a byte array which can be sent over the wire. Note that messages are only 15 bytes long
 * so this is not too bad.
 */
void MessageBuilder::toMessageArray(byte* result, Frame frame) {
	for (int i = 0; i < 15; i++) {
		result[i] = 0;
	}
	
	result[0] = 0;
	result[1] = ((byte) '0') + frame.source;
	result[2] = ((byte) '0') + frame.destination;
	result[3] = frame.messageType;
	result[4] = frame.userFunction;
	
	if (frame.functionCode.length() >=1) {
		result[5] = frame.functionCode[0];
	} else {
		result[5] = BLANK_FIELD;
	}
	
	if (frame.functionCode.length() >=2) {
		result[6] = frame.functionCode[1];
	} else {
		result[6] = BLANK_FIELD;
	}
	
	result[7] = (byte) frame.sign;
	
	if (frame.payload.length() >=1) {
		result[8] = frame.payload[0];
	} else {
		result[8] = BLANK_FIELD;
	}
	
	if (frame.payload.length() >=2) {
		result[9] = frame.payload[1];
	} else {
		result[9] = BLANK_FIELD;
	}
	
	if (frame.payload.length() >=3) {
		result[10] = frame.payload[2];
	} else {
		result[10] = BLANK_FIELD;
	}
	
	if (frame.payload.length() >=4) {
		result[11] = frame.payload[3];
	} else {
		result[11] = BLANK_FIELD;
	}
	
	result[12] = (byte) MESSAGE_END;
	
	byte* checksum = new byte[2];
	MessageBuilder::calculateCheckSum(checksum, result);
	result[13] = checksum[0];
	result[14] = checksum[1];
}

/**
 * Create a byte[] checksum of the input bytes. Note that this function only computes checksums for 13 byte arrays.
 */
void MessageBuilder::calculateCheckSum(byte* checksum, byte* message) {
	int sum;
	for (int i=0; i<13; i++) {
		sum += message[i];
	}
	
	checksum[0] = (sum >> 8) &255;
	checksum[1] = sum &255;
}