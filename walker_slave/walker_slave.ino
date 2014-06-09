#include <Modbus.h>
#include <Walker.h>
#include <Wire.h>

#define RS_CONTROL 5

/**
 * Command/control channel from Master.
 */
StreamHandler commandChannel(&Wire);
ModbusMaster master;
int myAddress;

const int ledPin =  13;      // the number of the LED pin
int ledState = LOW;             // ledState used to set the LED
unsigned long previousMillis = 0;        // will store last time LED was updated
const long interval = 100;           // interval at which to blink (milliseconds)

// Input buffer for meessages
char buffy[MAX_BUFFER] = {'\0'};
int currentSerialPos = 0;
bool midFlow = false; // Set to true once we have seen a : character prior to a newline
unsigned long serialPreviousMillis = 0;
const int serialTimeout = 1500;
bool messageRead = false; // Set to true when buffer may be read

void setup() {
  Serial.begin(9600);

  /**
   * Arduino address is set by pulling up inputs 2-4. This gives an address range of 0-7.
   */
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  pinMode(4, INPUT);

  // Pin your ears back.
  digitalWrite(RS_CONTROL, LOW);

  pinMode(ledPin, OUTPUT);

  myAddress = 1;
  //digitalRead(2) + (digitalRead(gitalRead(4) << 2);
  Wire.begin(myAddress);
  Wire.onReceive(receiveEvent); 
  resetMessage();
}

void loop() {
  unsigned long serialCurrentMillis = millis();

  if ((serialCurrentMillis - serialPreviousMillis) < serialTimeout) {
    serialPreviousMillis = serialCurrentMillis;
    // Not timed out
    if (Serial.available()) {
      char c = Serial.read();
      
//      // if the LED is off turn it on and vice-versa:
//      if (ledState == LOW)
//        ledState = HIGH;
//      else
//        ledState = LOW;

      // set the LED with the ledState of the variable:
//      digitalWrite(ledPin, ledState);
      
      if (':' == c) {
        midFlow = true;
        currentSerialPos = 0;
      } else if ('\r' == c) {
        messageRead = true;
      } else if (midFlow) {
          if (currentSerialPos < MAX_BUFFER) {
            buffy[currentSerialPos++] = c;
          } else {
            resetMessage();
          }
      } else {
        // Ignore it then
      }
    }
  } else {
    resetMessage();
  }

  if (messageRead) {
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
            if (result == 0) {
              //Success is an echo of the inbound message
              char msg[MAX_BUFFER] = {'\0'};
              master.toWireFormat(msg, message);
              digitalWrite(RS_CONTROL, HIGH);
              commandChannel.writeMessage(0, msg);
              delay(20);
              digitalWrite(RS_CONTROL, HIGH);
            } else {
               sendError(message, result);
            }
            break;
          }
        default:
          sendError(message, FUNCTION_NOT_SUPPORTED);
          break;
      }
    } else {
      // Ignore it then
      Serial.println("Not addressed to me");
    }
    
    resetMessage();
  }
}

void resetMessage() {
  currentSerialPos = 0;
  midFlow = false;
  messageRead = false;
  Serial.println("Reset");
  for (int i = 0; i < MAX_BUFFER; i++) {
    buffy[i] = '\0';
  }
  serialPreviousMillis = millis();
}

/**
 * Send an error message to the PC.
 * @param message The message to send an error about.
 * @param errorCode The error to return
 */
void sendError(MasterModbusMessage message, int errorCode) {
  char error[11] = {'\0'};
  master.toErrorFormat(error, message, errorCode);
  digitalWrite(RS_CONTROL, HIGH);
  commandChannel.writeMessage(0, error);
  digitalWrite(RS_CONTROL, LOW);
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

  switch (command) {
    case MOTION_HOME:
      digitalWrite(13, HIGH);
      break;
    case MOTION_FORWARD:
      for (int i = 0; i < 10; i++) {
        digitalWrite(13, HIGH);
        delay(250);
        digitalWrite(13, LOW);
        delay(250);
      }
      break;
    case MOTION_BACKWARD:
      for (int i = 0; i < 10; i++) {
        digitalWrite(13, HIGH);
        delay(100);
        digitalWrite(13, LOW);
        delay(100);
      }
      break;
    case MOTION_CROUCH:
      //      crouch();
      break;
    default:
      // OK, that didn't work then. Non-zero results are a failure so just send the command number back.
      result = command;
  }
  return result;
}
