void setup() {
  Serial.begin(9600);
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
}

void loop() {
  if (Serial.available()) {
    Serial.read();
    digitalWrite(13, HIGH);
    delay(50);
    digitalWrite(13, LOW);
  }
}
