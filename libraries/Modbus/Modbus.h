#include <Arduino.h>
#include <cstring>

#ifndef Modbus_h
#define Modbus_h

/**
 * A Modbus master message. These always have a four byte data payload. 
 * Functions except 0x41-0x48 and 0x64-0x6e are well defined in the standard. The 17 other
 * messages may be user defined.
 */
typedef struct MasterModbusMessage {
	byte slave;
	byte function;
	byte data1;
	byte data2;
	byte data3;
	byte data4;
	bool failedLrc;
} MasterModbusMessage;

class ModbusMaster {
	public:
		ModbusMaster();
		
		/**
		 * Converts a Modbus struct into a char buffer.
		 * @param buffer The buffer to populate, make sure you have space
		 * @param message The message to convert 
		 */
		void toWireFormat(char* buffer, MasterModbusMessage message);
		
		/**
		 * Converts a Modbus struct into a char buffer as an error message.
		 * @param buffer The buffer to populate, make sure you have space
		 * @param message The message to convert 
		 * @param errorCode The error code to send
		 */
		void toErrorFormat(char* buffer, MasterModbusMessage message, int errorCode);
		
		/**
		 * Converts an ascii Modbus message into a Modbus message struct.
		 * @param buffer The buffer to read, make sure you have space
		 * @param message The message to populate 
		 */
		void fromWireFormat(MasterModbusMessage* message, char* buffer);
	private:
		/**
		 * Calculate the checksum of a char buffer for a length of bytes given.
		 */
		int calculateChecksum(char* buffer, int bytes);
};

class StreamHandler {
	public:
		StreamHandler(Stream* _stream);
		/**
		 * Read data from the given stream, packaging it into the given buffer. Make sure
		 * you allocate sufficient space in the buffer
		 */
		int readMessage(char* buffer, int bufferSize);
		/**
		 * Write the data given in buffer to the given stream.
		 */
		void writeMessage(char* buffer);
	private:
		Stream* stream;
};

#endif
