#include <Wire.h>

const char *COMMAND_HOME = "HOME";
char *delimiter = "|";
const int buffer_size = 100;

// How many legs do we have?
const int maxLegs = 1;

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
      char *additionalInformation;

      bool success;
      if (strcmp(COMMAND_HOME, command) == 0) {
        success = sendHome();
      } else {
        success = false;
        additionalInformation = "NOT_SUPPORTED";
      }

      char *format = "";
      if (success == true) {
        format = "SUCCESS|%s|%s|%s";
      } else {
        format = "FAILED|%s|%s|%s";
      }
      char response[180];
      sprintf(response, format, correlationId, command, additionalInformation);
      Serial.println(response);

    } else {
      char response[120];
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
