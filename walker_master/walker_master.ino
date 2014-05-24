#include <Modbus.h>

#define MAX_BUFFER 100

#define NUM_LEGS 6

/** 
 * Function code errors.
 */
#define FUNCTION_NOT_SUPPORTED 1
#define REGISTER_OUT_OF_RANGE  2
#define REGISTER_NOT_AVAILABLE 3
#define WRITE_SINGLE_REGISTER_FAIL 4

/**
 * Motion commands
 */
#define MOTION_HOME 1
#define MOTION_FORWARD 2
#define MOTION_BACKWARD 3
#define MOTION_LEFT 4
#define MOTION_RIGHT 5
#define MOTION_CROUCH 6

/**
 * Supported function codes.
 */
#define WRITE_SINGLE_REGISTER 6

/** 
 * Registers
 */
#define MOTION_REGISTER 1
/**
 * Command/control channel from PC.
 */
StreamHandler commandChannel(&Serial);
ModbusMaster master;
int myAddress;

/**
 * Command/control channel for legs.
 */
StreamHandler legChannel(&Serial1);

void setup() {
    
  // Main command channel
  Serial.begin(9600);
  
  // Leg control channel
  Serial1.begin(9600);
  
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
  char buffy[MAX_BUFFER] = {'\0'};  
  
  commandChannel.readMessage(buffy, MAX_BUFFER);
  
  MasterModbusMessage message;
  master.fromWireFormat(&message, buffy);
  
  if (message.failedLrc) {
    // Ignore it then.
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
          commandChannel.writeMessage(msg);
        }
        break;
      }
      default:
        sendError(message, FUNCTION_NOT_SUPPORTED);
        break;
    }
  } else {
    // Ignore it then
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
  commandChannel.writeMessage(error);
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
    case MOTION_HOME:{
      allStop();
      break;
    }
    case MOTION_FORWARD:{
      leftForward(speed);
      rightForward(speed);
      break;
    } 
    case MOTION_BACKWARD:{
      leftBackward(speed);
      rightBackward(speed);
      break;
    }
    case MOTION_LEFT:{
      leftBackward(50);
      rightForward(50);
      break;
    }
    case MOTION_RIGHT:{
      leftForward(50);
      rightBackward(50);
      break;
    }
    case MOTION_CROUCH:{
//      crouch();
      break;
    }
    default:
      // OK, that didn't work then
      result = command;
  }
  
  return result;
}

/**
 * Issue the all stop.
 */
void allStop() {
  for (int i=0; i<NUM_LEGS; i++) {
    sendLegMessage(i, MOTION_HOME, 0);
  }
}

/**
 * Left side of bug forwards. Left legs are even numbered.
 */
void leftForward(byte speed) {
  for (int i=0; i<NUM_LEGS; i+=2) {
    sendLegMessage(i, MOTION_FORWARD, speed);
  }
}

/**
 * Right side of bug forwards. Right legs are odd numbered.
 */
void rightForward(byte speed) {
  for (int i=1; i<NUM_LEGS; i+=2) {
    sendLegMessage(i, MOTION_FORWARD, speed);
  }
}

/**
 * Left side of bug backwards. Left legs are even numbered.
 */
void leftBackward(byte speed) {
  for (int i=0; i<NUM_LEGS; i+=2) {
    sendLegMessage(i, MOTION_BACKWARD, speed);
  }
}

/**
 * Right side of bug backwards. Right legs are odd numbered.
 */
void rightBackward(byte speed) {
  for (int i=1; i<NUM_LEGS; i+=2) {
    sendLegMessage(i, MOTION_BACKWARD, speed);
  }
}


/**
 * Send a motion message to a leg.
 * @param leg The leg
 * @param action The action to perform
 * @param speed The speed to perform it at
 */
void sendLegMessage(int leg, byte action, byte speed) {
  MasterModbusMessage message;
  message.slave = leg;
  message.function = WRITE_SINGLE_REGISTER;
  message.data1 = 0;
  message.data2 = MOTION_REGISTER;
  message.data3 = speed;
  message.data4 = action;
  char buffer[18] = {'\0'};
  master.toWireFormat(buffer, message);   
  legChannel.writeMessage(buffer);
}
