#include <Wire.h>

void setup() {
  Wire.begin();
  Wire.setTimeout(500);
}

void loop() {
  Wire.beginTransmission(1); // transmit to device #4
  Wire.write("H");        // sends five bytes
  Wire.endTransmission();    // stop transmitting

  delay(5000);
  
  Wire.beginTransmission(1); // transmit to device #4
  Wire.write("F");        // sends five bytes
  Wire.endTransmission();    // stop transmitting
  
  delay(5000);
}
