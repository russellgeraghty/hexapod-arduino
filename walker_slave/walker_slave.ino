bool dark = true;

void setup() {
  Serial.begin(9600);
  pinMode(13, OUTPUT);
}

void loop() {
  if (Serial.available()) {
    Serial.read();
    if (dark) {
      digitalWrite(13, HIGH);
    } else {
      digitalWrite(13, LOW);  
    }
    dark = !dark;
  }
}
