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

    String additionalInformation;

    int delimiters = StringUtils::countSplitCharacters(message, delimiter);
    if (delimiters >= 1) {
      String pieces[delimiters + 1];
      int count = StringUtils::split(message, delimiter, pieces);

      String correlationId = pieces[0];
      String command = pieces[1];
      
      Serial.println(correlationId);
      Serial.println(command);

      bool success;

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
    success &= (sendLegCommand(i, 'H') == 0);
  }
  return success;
}

/**
 * Send a command to a leg.
 * @return non-zero response in case of failure
 */
int sendLegCommand(int leg, char command) {
  Wire.beginTransmission(leg);
  Wire.write(command);
  int response = Wire.endTransmission();
  return response;
}
