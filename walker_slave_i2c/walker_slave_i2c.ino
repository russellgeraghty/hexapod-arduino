#include <Modbus.h>
#include <Walker.h>
#include <Wire.h>

int myAddress;

// The inbound request
MasterModbusMessage requestCommand;

// The command we're currently running
MasterModbusMessage runningCommand;

// Has the currentReply been sent over i2c already?
bool replySentAlready;

// The current reply
MasterModbusMessage currentReply;

void setup() {
  Serial.begin(9600);
  // TODO - Read from 2-4
  myAddress = 1;
  Wire.begin(myAddress);
  // Go home
  runningCommand.slave = myAddress;
  runningCommand.function = MOTION_HOME;
  runningCommand.data1 = 0;
  runningCommand.data2 = 0;
  runningCommand.data3 = 0;
  runningCommand.data4 = 0;
  runningCommand.failedLrc = false;

  // Current reply has no value, so pretend we've already sent it
  replySentAlready = true;

  Wire.onReceive(handleInboundMessage);
  Wire.onRequest(handleOutboundMessage);
}


void loop() {
  if (isSameMessage(runningCommand, requestCommand)) {
    handleRunningCommand(runningCommand);
  } else {
    // OK, lets swap them over then
    runningCommand = requestCommand;
  }
}

// Handle the current message
void handleRunningCommand(MasterModbusMessage message) {
}

/**
 * Handle an inbound message.
 */
void handleInboundMessage(int howMany) {
  //Serial.println("Inbound message");
  char b[MAX_BUFFER] = {'\0'};
  int read = Wire.readBytesUntil('\n', b, MAX_BUFFER);

  if (read > 0) {
    char buffy[MAX_BUFFER] = {'\0'};
    char c = '\0';
    int count = 0;
    bool reading = false;

    for (int i = 0; i < read && count < read; i++) {
      c = b[i];
      if (':' == c) {
        reading = true;
        // Go back to the start of the buffer
        count = 0;
        Serial.println("Begin message");
      } else if (reading && ( (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'))) {
        buffy[count++] = c;
      }
    }

    Serial.print("Buffer is ");
    Serial.println(buffy);

    MasterModbusMessage message;
    ModbusMaster master;
    master.fromWireFormat(&message, buffy);

    if (message.failedLrc) {
      // Ignore it then.
      Serial.println("Failed LRC");
    } else if (message.slave == myAddress) {

      // TODO - handle validation of command
      requestCommand = message;

      // Success is a reply to the message
      currentReply = message;
      replySentAlready = false;
    }
  }
}


void handleOutboundMessage() {
  if (replySentAlready) {
    // Ignore the repeated request for data
  } else {
    //Success is an echo of the inbound message
    char msg[MAX_BUFFER];
    for (int i = 0; i < MAX_BUFFER; i++) {
      msg[i] = '\0';
    }
    ModbusMaster master;
    master.toWireFormat(msg, currentReply);
    Wire.write(msg);
    replySentAlready = true;
  }
}

/**
 * Compare two messages.
 */
bool isSameMessage(MasterModbusMessage msg1, MasterModbusMessage msg2) {
  bool equal = true;
  equal &= (msg1.slave == msg2.slave);
  equal &= (msg1.function == msg2.function);
  equal &= (msg1.data1 == msg2.data1);
  equal &= (msg1.data2 == msg2.data2);
  equal &= (msg1.data3 == msg2.data3);
  equal &= (msg1.data4 == msg2.data4);
  equal &= (msg1.failedLrc = msg2.failedLrc);
  return equal;
}
