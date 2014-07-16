#include <Wire.h>

const char *COMMAND_HOME  = "HOME";
const char *COMMAND_WALK  = "FORWARD";
const char *COMMAND_BACK  = "BACKWARD";
const char *COMMAND_LEFT  = "LEFT";
const char *COMMAND_RIGHT = "RIGHT";

const char LEG_HOME     = 'H';
const char LEG_FORWARD  = 'F';
const char LEG_BACKWARD = 'B';

char *delimiter = "|";
const int buffer_size = 100;

// How many legs do we have?
const int maxLegs = 6;

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(500);
  Wire.begin();
  Wire.setTimeout(500);
}

void loop() {
  char buf[buffer_size];
  int number = getline(buf, buffer_size);

  if (number > 0) {
    int pos = 0;
    char *pieces[3];
    char *ch = strtok(buf, delimiter);

    while (ch != NULL && pos  < 3) {
      pieces[pos++] = ch;
      ch = strtok(NULL, delimiter);
    }

    if (pos > 1) {
      char *correlationId = pieces[0];
      char *command = pieces[1];
      char *additionalInformation = "";

      bool success;
      if (strcmp(COMMAND_HOME, command) == 0) {
        success = sendHome(correlationId);
      } else if (strcmp(COMMAND_WALK, command) == 0) {
        success = sendForward(correlationId);
      } else if (strcmp(COMMAND_BACK, command) == 0) {
        success = sendBackward(correlationId);
      } else if (strcmp(COMMAND_LEFT, command) == 0) {
        success = sendLeft(correlationId);
      } else if (strcmp(COMMAND_RIGHT, command) == 0) {
        success = sendRight(correlationId);
      } else {
        success = false;
        additionalInformation = "NOT_SUPPORTED";
      }

      char *format = "";
      if (success == true) {
        format = "%s|SUCCESS|%s|%s";
      } else {
        format = "%s|FAILED|%s|%s";
      }
      char response[180];
      sprintf(response, format, correlationId, command, additionalInformation);
      Serial.println(response);

    } else {
      char response[30];
      sprintf(response, "FAILED|MESSAGE_TOO_SHORT|%d", pos);
      Serial.println(response);
    }
  }
}

/**
 * Get a line of text and put it into the supplied s[], limited to the given length.
 * @param s The value to return
 * @param lim The limit of chars to get (the size of s[])
 * @return The number of characters actually received
 */
int getline(char s[], int lim) {
  int length = 0;
  if (Serial.available() > 0) {
    length = Serial.readBytesUntil('\n', s, lim - 1);
  }
  s[length] = '\0';
  return length;
}


/**
 * Ask all the legs to go home.
 * @param correlationId which command instigated the motion
 * @return true if all the legs responded properly, false if not
 */
bool sendHome(char* correlationId) {
  bool success = true;
  for (int i = 1; i <= maxLegs; i++) {
    success &= (sendLegCommand(correlationId, i, LEG_HOME) == 0);
  }
  return success;
}

/**
 * Ask all the legs to go forwards.
 * @param correlationId which command instigated the motion
 * @return true if all the legs responded properly, false if not
 */
bool sendForward(char* correlationId) {
  bool success = true;
  for (int i = 1; i <= maxLegs; i++) {
    success &= (sendLegCommand(correlationId, i, LEG_FORWARD) == 0);
  }
  return success;
}

/**
 * Ask all the legs to go backwards.
 * @param correlationId which command instigated the motion
 * @return true if all the legs responded properly, false if not
 */
bool sendBackward(char* correlationId) {
  bool success = true;
  for (int i = 1; i <= maxLegs; i++) {
    success &= (sendLegCommand(correlationId, i, LEG_BACKWARD) == 0);
  }
  return success;
}

/**
 * Ask all the odd legs (on the left) to go forwards. All the even legs go backwards.
 * @param correlationId which command instigated the motion
 * @return true if all the legs responded properly, false if not
 */
bool sendLeft(char* correlationId) {
  bool success = true;
  for (int i = 1; i <= maxLegs; i++) {
    char command;
    if (i % 2 == 1) {
      command = LEG_BACKWARD;
    } else {
      command = LEG_FORWARD;
    }

    success &= (sendLegCommand(correlationId, i, command) == 0);
  }
  return success;
}

/**
 * Ask all the odd legs (on the left) to go backwards. All the even legs go forwards.
 * @param correlationId which command instigated the motion
 * @return true if all the legs responded properly, false if not
 */
bool sendRight(char* correlationId) {
  bool success = true;
  for (int i = 1; i <= maxLegs; i++) {
    char command;
    if (i % 2 == 1) {
      command = LEG_FORWARD;
    } else {
      command = LEG_BACKWARD;
    }

    success &= (sendLegCommand(correlationId, i, command) == 0);
  }
  return success;
}

/**m
 * Print some serial debug out.
 *
 * @param correlationId the command reference to which feedback applies
 * @param leg the leg we spoke to
 * @param command the command we sent
 * @param success successful or not?
 */
void debug(char* correlationId, int leg, char command, int success) {
  char *format = "%s|INFO|%c|Sent to leg %d with response %d";
  char buffer[120];
  
  sprintf(buffer, format, correlationId, command, leg, success);
  Serial.println(buffer);
}

/**
Mm-0:success 
-1:data too long to fit in transmit buffer 
-2:received NACK on transmit of address 
-3:received NACK on transmit of data 
-4:other error 
*/

/**
 * Send a command to a leg.
 * @param correlationId the comamnd reference from the client
 * @param leg the leg number
 * @param command the command to send
 * @return non-zero response in case of failure
 */
int sendLegCommand(char *correlationId, int leg, char command) {
  Wire.beginTransmission(leg);
  Wire.write(command);
  int response = Wire.endTransmission();
  
  debug(correlationId, leg, command, response);
  
  return response;
}
