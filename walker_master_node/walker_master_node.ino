#include <Wire.h>

const String COMMAND_HOME = "HOME";

// How many legs do we have?
const int maxLegs = 1;

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(500);
  Wire.begin();
  Wire.setTimeout(500);
}

void loop() {
  if (Serial.available() > 0) {
    String message = Serial.readStringUntil('\n');
    message.trim();
    int barIndex = message.indexOf('|');
    String command = message.substring(0, barIndex);

    bool success;
    String additionalInformation = "";

    if (COMMAND_HOME == command) {
      success = sendHome();
    } else {
      success = false;
      additionalInformation = "NOT_SUPPORTED";
    }

    if (success) {
      Serial.println("SUCCESS|" + command + "|" + additionalInformation);
    } else {
      Serial.println("FAILED|" + command + "|" + additionalInformation);
    }

  }
}

/**
 * Ask all the legs to go home.
 * @return true if all the legs responded properly, false if not
 */
bool sendHome() {
  bool success = true;
  for (int i = 1; i <= maxLegs; i++) {
    success &= (sendLegCommand(i, COMMAND_HOME, NULL) == 0);
  }
  return success;
}

/**
 * Send a command to a leg.
 * @return non-zero response in case of failure
 */
int sendLegCommand(int leg, String command, String args[]) {
  Wire.beginTransmission(leg);
  char buffer[command.length() + 1];
  command.toCharArray(buffer, command.length() + 1);
  Wire.write(buffer);
  
  for (int i = 0; i < sizeof(args)/sizeof(args[0]); i++ ) {
    String arg = args[i];
    char buff[arg.length() + 1];
    arg.toCharArray(buff, arg.length() + 1);
    Wire.write(buff);
  }
  Wire.write('\n');
  
  int response = Wire.endTransmission();
  return response;
}
