#include <Wire.h>
#include <StringUtils.h>

const String COMMAND_HOME = "HOME";
const char delimiter = '|';

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

    int delimiters = StringUtils::countSplitCharacters(message, delimiter);
    if (delimiters >= 1) {
      String* pieces = StringUtils::split(message, delimiter);

      String correlationId = pieces[0];
      String command = pieces[1];

      bool success;
      String additionalInformation = "";

      if (COMMAND_HOME == command) {
        success = sendHome();
      } else {
        success = false;
        additionalInformation = "NOT_SUPPORTED";
      }

      if (success) {
        Serial.println("SUCCESS|" + correlationId + "|" + command + "|" + additionalInformation);
      } else {
        Serial.println("FAILED|" + correlationId + "|" + command + "|" + additionalInformation);
      }
    } else {
      Serial.print("FAILED|MESSAGE_TOO_SHORT|");
      Serial.println(delimiters + 1);
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

  for (int i = 0; i < sizeof(args) / sizeof(args[0]); i++ ) {
    String arg = args[i];
    char buff[arg.length() + 1];
    arg.toCharArray(buff, arg.length() + 1);
    Wire.write(buff);
  }
  Wire.write('\n');

  int response = Wire.endTransmission();
  return response;
}
