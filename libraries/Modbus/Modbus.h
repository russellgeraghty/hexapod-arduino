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
} MasterModbusMessage;

class ModbusMaster {
	public:
		ModbusMaster();
		void toWireFormat(char* buffer, MasterModbusMessage message);
		void fromWireFormat(MasterModbusMessage* message, char* buffer);
	private:
		int calculateChecksum(char* buffer, int bytes);
};

#endif
