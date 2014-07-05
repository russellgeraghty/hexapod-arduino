#include <Wire.h>
#include <Servo.h>

#define LED_PIN 13
#define WAIST_PIN 9
#define SHOULDER_PIN 10
#define ELBOW_PIN 11

/**
 * Commands are a single letter.
 * F - Go forwards
 * R - Go rearwards
 * C - Crouch
 * H - Go to home position
 */

char standingOrder = 'F';

// The previus time the leg motion was changed
unsigned long previousMillis = 0;
unsigned long ledIllumiteUntil = 0;

// Interval in milliseconds between leg actions:
const long interval = 100;

// This is the home position, a single standing leg
// Order is waist, shoulder, elbow
short home[] = {90, 90, 90, -1};
short forwards[] = {80, 90, 90, 80, 120, 90, 120, 120, 90, 120, 90, 90, -1};

// The current index in the position loop
byte index = 0;

// We have thres joints to move
const byte numJoints = 3;

// The current motion being actioned
short *motion;

Servo waist;
Servo shoulder;
Servo elbow;

void setup()
{
  Serial.begin(9600);           // start serial for output

  // Use the Analogue pins to set the bus address
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  digitalWrite(A0, HIGH);
  digitalWrite(A1, HIGH);
  digitalWrite(A2, HIGH);
  int channel = digitalRead(A0) + (digitalRead(A1) << 1) + (digitalRead(A2) << 2);

  // Servos
  waist.attach(WAIST_PIN);
  shoulder.attach(SHOULDER_PIN);
  elbow.attach(ELBOW_PIN);
  
  // LED
  pinMode(LED_PIN, OUTPUT);

  Wire.begin(channel);
  Wire.onReceive(receiveEvent); // register event
  Wire.setTimeout(500);

  setStandingOrder();
}

void loop()
{
  // Handle the LED
  
  unsigned long currentLedMillis = millis();
  
  if (currentLedMillis > ledIllumiteUntil) {
    digitalWrite(LED_PIN, LOW);
  }
  
  // Every 100ms read the increment position (or go back home) and send the new leg options out
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    short waist = motion[index];
    short shoulder = motion[index + 1];
    short elbow = motion[index + 2];

    setWaist(waist);
    setShoulder(shoulder);
    setElbow(elbow);

    // Now change the leg index counter
    index += numJoints;
    // Because a joint can be at postion 0 perfectly legally the motion index array is terminated by -1.
    if (motion[index] == -1) {
      Serial.print("Next index ");
      Serial.print(index);
      Serial.print(" is the end of motion, resetting");
      Serial.println();
      index = 0;
    }

  }
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany)
{
  char myOrder = standingOrder;
  for (int i = howMany; i > 0; i--) {
    // Just read all the commands
    standingOrder = Wire.read();
  }
  // Don't admit we've changed until all bytes have been read.

  if (standingOrder != myOrder) {
    Serial.print("Received new command: ");
    Serial.println(standingOrder);

    // Update the current pointer to the leg motion setup
    setStandingOrder();
  }
}

void setWaist(short waistPosition) {
  Serial.print("Requested waist to    ");
  Serial.print(waistPosition);
  Serial.println();
  
  waistPosition = min(100, waistPosition);
  waistPosition = max(80, waistPosition);
  
  Serial.print("Setting waist to      ");
  Serial.print(waistPosition);
  Serial.println();  
  
  waist.write(waistPosition);
}

void setShoulder(short shoulderPosition) {
  Serial.print("Requested shoulder to ");
  Serial.print(shoulderPosition);
  Serial.println();
  
  shoulderPosition = min(130, shoulderPosition);
  shoulderPosition = max(50, shoulderPosition);
  
  Serial.print("Setting shoulder to   ");
  Serial.print(shoulderPosition);
  Serial.println();
  
  shoulder.write(shoulderPosition);
}

void setElbow(short elbowPosition) {
  Serial.print("Requested elbow to    ");
  Serial.print(elbowPosition);
  Serial.println();
  
  elbowPosition = min(150, elbowPosition);
  elbowPosition = max(30, elbowPosition);
  
  Serial.print("Setting elbow to      ");
  Serial.print(elbowPosition);
  Serial.println();
  
  elbow.write(elbowPosition);  
}

/**
 * Set the standing order we're working with
 * @param theOrder The command to move over to
 */
void setStandingOrder() {
  int changed = false;

  Serial.print("Received command ");
  Serial.println(standingOrder);

  switch (standingOrder) { 
    case 'H':
      motion = home;
      changed = true;
      break;
    case 'F':
      motion = forwards;
      changed = true;
      break;
    default:
      Serial.print("Received command ");
      Serial.print(standingOrder);
      Serial.println(" which was not understood");
      break;
  }

  if (changed) {
    index = 0;
    ledIllumiteUntil = millis() + 500;
    digitalWrite(LED_PIN, HIGH);
  }
}

