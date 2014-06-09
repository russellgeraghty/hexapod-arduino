#include <Modbus.h>
#include <Walker.h>
#include <Wire.h>

/**
 * Command/control channel from PC.
 */

ModbusMaster master;
int myAddress;

/**
 * Command/control channel for legs.
 */
StreamHandler legChannel(&Wire);

void setup() {

  // Main command channel
  Serial.begin(9600);

  // Leg control channel
  Wire.begin();

  Serial.setTimeout(1500);

  /**
   * Arduino address is set by pulling up inputs 2-4. This gives an address range of 0-7.
   */
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  pinMode(4, INPUT);

  myAddress = digitalRead(2) + (digitalRead(3) << 1) + (digitalRead(4) << 2);
}

/**
 * Sit in a loop reading from Serial, this is the command channel from the controller.
 * Then, when a command comes in speak to the legs, translating the inbound command into the
 * correct sequence for the legs.
 */
void loop() {
  char c = '\0';
  int count = 0;
  bool reading = false;

  char b[MAX_BUFFER] = {'\0'};
  int read = Serial.readBytesUntil('\n', b, MAX_BUFFER);

  if (read > 0) {
    char buffy[MAX_BUFFER] = {'\0'};

    for (int i = 0; i < read && count < read; i++) {
      c = b[i];
      if (':' == c) {
        reading = true;
        // Go back to the start of the buffer
        count = 0;
      } else if (reading && ( (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'))) {
        buffy[count++] = c;
      }
    }

    Serial.println(buffy);

    MasterModbusMessage message;
    master.fromWireFormat(&message, buffy);

    if (message.failedLrc) {
      // Ignore it then.
      Serial.println("Failed LRC");
    } else if (message.slave == myAddress) {
      switch (message.function) {
        case WRITE_SINGLE_REGISTER: {
            int reg = message.data2 + (message.data1 << 16);
            int result = handleRegisterWrite(reg, message.data3, message.data4);
            if (result != 0) {
              sendError(message, result);
            } else {
              //Success is an echo of the inbound message
              char msg[MAX_BUFFER] = {'\0'};
              master.toWireFormat(msg, message);
              Serial.println(msg);
            }
            break;
          }
        default:
          sendError(message, FUNCTION_NOT_SUPPORTED);
          break;
      }
    } else {
      Serial.print("Not addressed to me ");
      Serial.println(message.slave);
    }
  }
}

/**
 * Send an error message to the PC.
 * @param message The message to send an error about.
 * @param errorCode The error to return
 */
void sendError(MasterModbusMessage message, int errorCode) {
  char error[11] = {'\0'};
  master.toErrorFormat(error, message, errorCode);
  Serial.println(error);
}

/**
 * Handle sending a register message.
 * @param reg The register number.
 * @param data The data to write.
 * @return Non-zero error code if the register or data are out of bounds.
 */
int handleRegisterWrite(int reg, byte data1, byte data2) {
  int result = 0;
  switch (reg) {
    case MOTION_REGISTER: {
        byte command = data2;
        byte spd = data1;
        int response = handleMotionCommand(command, spd);
        if (response != 0) {
          result = WRITE_SINGLE_REGISTER_FAIL;
        }
        break;
      }

    default:
      result = REGISTER_NOT_AVAILABLE;
      break;
  }

  return result;
}

/**
 * Handle a motion command.
 * @param command - the command to execute
 * @param speed - the speed at which to move. This parameter may have no effect.
 * @return A non-zero result indicates failure. Probably requested a command which is not available.
 */
int handleMotionCommand(byte command, byte speed) {
  int result = 0;

  bool success = true;

  switch (command) {
    case MOTION_HOME:
      success &= allStop();
      break;
    case MOTION_FORWARD:
      success &= leftForward(speed);
      success &= rightForward(speed);
      break;
    case MOTION_BACKWARD:
      success &= leftBackward(speed);
      success &= rightBackward(speed);
      break;
    case MOTION_LEFT:
      success &= leftBackward(50);
      success &= rightForward(50);
      break;
    case MOTION_RIGHT:
      success &= leftForward(50);
      success &= rightBackward(50);
      break;
    case MOTION_CROUCH:
      //      crouch();
      break;
    default:
      // OK, that didn't work then. Non-zero results are a failure so just send the command number back.
      result = command;
  }

  if (false == success) {
    result = 64;
  }

  return result;
}

/**
 * Issue the all stop.
 */
bool allStop() {
  bool success = true;
  for (int i = 1; i <= NUM_LEGS; i++) {
    success &= sendLegMessage(i, MOTION_HOME, 0);
  }
  return success;
}

/**
 * Left side of bug forwards. Left legs are even numbered.
 */
bool leftForward(byte speed) {
  bool success = true;
  for (int i = 1; i <= NUM_LEGS; i += 2) {
    success &= sendLegMessage(i, MOTION_FORWARD, speed);
  }
  return success;
}

/**
 * Right side of bug forwards. Right legs are odd numbered.
 */
bool rightForward(byte speed) {
  bool success = true;
  for (int i = 2; i <= NUM_LEGS; i += 2) {
    success &= sendLegMessage(i, MOTION_FORWARD, speed);
  }
  return success;
}

/**
 * Left side of bug backwards. Left legs are even numbered.
 */
bool leftBackward(byte speed) {
  bool success = true;
  for (int i = 1; i <= NUM_LEGS; i += 2) {
    success &= sendLegMessage(i, MOTION_BACKWARD, speed);
  }
  return success;
}

/**
 * Right side of bug backwards. Right legs are odd numbered.
 */
bool rightBackward(byte speed) {
  bool success = true;
  for (int i = 2; i <= NUM_LEGS; i += 2) {
    success &= sendLegMessage(i, MOTION_BACKWARD, speed);
  }
  return success;
}


/**
 * Send a motion message to a leg.
 * @param leg The leg
 * @param action The action to perform
 * @param speed The speed to perform it at
 * @return true if successful, false otherwise
 */
bool sendLegMessage(int leg, byte action, byte speed) {
  MasterModbusMessage message;
  message.slave = leg;
  message.function = WRITE_SINGLE_REGISTER;
  message.data1 = 0;
  message.data2 = MOTION_REGISTER;
  message.data3 = speed;
  message.data4 = action;
  char buffer[20];
  for (int i = 0; i < 20; i++) {
    buffer[i] = '\0';
  }
  
  master.toWireFormat(buffer, message);

  Serial.println("Writing message ");
  bool result = legChannel.writeMessage(leg, buffer);
  Serial.print("Wrote message ");
  Serial.print(buffer);
  Serial.print(" to ");
  Serial.println(leg);
  
  bool success = false;
  
  if (result) {
    char inputBuffer[MAX_BUFFER] = {'\0'};
    int read = legChannel.readMessage(leg, inputBuffer, MAX_BUFFER);
    Serial.print("Read ");
    Serial.println(read);
    Serial.print("Buffer [");
    Serial.print(inputBuffer);
    Serial.println("]");

    success = true;
    for (int i = 0; i < 18; i++) {
      success &= (buffer[i] == inputBuffer[i]);
    }
  }

  return success;
}
