#include <Wire.h>
#include <Servo.h>

#define LED_PIN 13
#define COXA_PIN 9
#define TROCANTERE_PIN 10
#define PATELLA_PIN 11
#define FEMUR 95
#define TIBIA 125
#define COXA_L 35

/**
 * Commands are a single letter.
 * F - Go forwards
 * R - Go rearwards
 * C - Crouch
 * H - Go to home position
 */

char standingOrder = 'H';

// The previus time the leg motion was changed
unsigned long previousMillis = 0;
unsigned long ledIllumiteUntil = 0;

// Interval in milliseconds between leg actions:
const long INTERVAL = 500;

const short femurSquared = FEMUR*FEMUR;
const short tibiaSquared = TIBIA*TIBIA;
const short coxaSquared = COXA_L*COXA_L;

boolean isLeft = false;

// This is the home position, a single standing leg. All distances in mm.
// Order is x, y, z. z is the height of the body above ground, x runs across the 
//  join of the abdomen (were there to be one). y is in the direction of travel
//  for something moving forwards.
double home[] = {130, 0, 125, -1};

// A walking motion
double forwards[] = {130, 0, 125,
                     130, 20, 125,
                     130, 40, 125,
                     130, 60, 125,
                     130, 40, 125,
                     130, 20, 125,
                    -1};

// The current index in the position loop
byte index = 0;

// We have thres joints to move
const byte numJoints = 3;

// The current motion being actioned
double *motion;

Servo coxa;
Servo trocantere;
Servo patella;

int channel;

void setup()
{
  // start serial for output
  Serial.begin(9600);           

  // Use the Analogue pins to set the bus address
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  digitalWrite(A0, HIGH);
  digitalWrite(A1, HIGH);
  digitalWrite(A2, HIGH);
  channel = digitalRead(A0) + (digitalRead(A1) << 1) + (digitalRead(A2) << 2);

  // Is my leg on the left hand side?
  isLeft = channel < 4;

  // Servos
  coxa.attach(COXA_PIN);
  trocantere.attach(TROCANTERE_PIN);
  patella.attach(PATELLA_PIN);
  
  // LED
  pinMode(LED_PIN, OUTPUT);
  
  Wire.begin(channel);
  Wire.onReceive(receiveEvent); // register event
  Wire.setTimeout(500);

  setStandingOrder();
}

void loop()
{
  double angles[] = {0,0,0};
  
  // Handle the LED
  unsigned long currentLedMillis = millis();
  
  if (currentLedMillis > ledIllumiteUntil) {
    digitalWrite(LED_PIN, LOW);
  }
  
  // Every so often read the increment position (or go back home) and send the new leg options out
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= INTERVAL) {
    previousMillis = currentMillis;

    calculateAngles(&motion[index], angles);

    double coxa_degrees = angles[0];
    double trocantere_degrees = angles[1];
    double patella_degrees = angles[2];

    setCoxa(coxa_degrees);
    setTrocantere(trocantere_degrees);
    setPatella(patella_degrees);

    // Now change the leg index counter
    index += numJoints;
    // Because a joint can be at postion 0 perfectly legally the motion index array is terminated by -1.
    if (motion[index] == -1) {
      index = 0;
    }

  }
  
  // On serial read send out some debug
  int theNumber;
  if (theNumber = Serial.available()) {
    for (int i = 0; i < theNumber; i++) {
      Serial.read();
    }
    
    Serial.print("Listening on channel [");
    Serial.print(channel);
    Serial.print("]. Currently executing command [");
    Serial.print(standingOrder);
    Serial.print("]");
    Serial.print(" Left? ");
    Serial.print(isLeft);
    Serial.println();
  }
}

/**
 * Calculate all the angles from the given motion.
 * 
 * @param double* motion - pointer to an array of motions, at least three wide.
 *   Note: This function will not check that the array is wide enough.
 * @param double* angles - an array into which we place the calculated values
 *   Note: The caller must ensure this is initialised and wide enough
 */
void calculateAngles(double* motion, double* angles) {
  double x = motion[0];
  double y = motion[1];
  double z = motion[2];

  double gamma = radiansToDegrees(atan(y/(x - COXA_L)));

  double lPrime = sqrt( sq(x - COXA_L) + sq(z) );
  double lPrimeSquared = sq(lPrime);
  double beta = radiansToDegrees(acos( (femurSquared + tibiaSquared - lPrimeSquared)/(2 * FEMUR * TIBIA)) );
  double alpha = radiansToDegrees(acos(z/lPrime) + acos( (femurSquared + lPrimeSquared - tibiaSquared)/(2 * FEMUR * lPrime)));
  
  angles[0] = gamma;
  angles[1] = alpha;
  angles[2] = beta;
}

double radiansToDegrees(double rads) {
  return (rads * 4068) / 71;
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

void setCoxa(short coxaPosition) {
//  Serial.print("Requested coxa to       ");
//  Serial.print(coxaPosition);
//  Serial.println();

  coxaPosition = coxaPosition + 90;
  coxaPosition = min(110, coxaPosition);
  coxaPosition = max(70, coxaPosition);
  
  Serial.print("Setting coxa to         ");
  Serial.print(coxaPosition);
  Serial.println();  
  
  coxa.write(coxaPosition);
}

void setTrocantere(short trocanterePosition) {
//  Serial.print("Requested trocantere to ");
//  Serial.print(trocanterePosition);
//  Serial.println();
 
  trocanterePosition = trocanterePosition;
  trocanterePosition = min(180, trocanterePosition);
  trocanterePosition = max(0, trocanterePosition);
  
  Serial.print("Setting trocantere to   ");
  Serial.print(trocanterePosition);
  Serial.println();
  
  trocantere.write(trocanterePosition);
}

void setPatella(short patellaPosition) {
//  Serial.print("Requested patella to    ");
//  Serial.print(patellaPosition);
//  Serial.println();

  patellaPosition = patellaPosition;
  
  patellaPosition = min(180, patellaPosition);
  patellaPosition = max(0, patellaPosition);

  Serial.print("Setting patella to      ");
  Serial.print(patellaPosition);
  Serial.println();
  
  patella.write(patellaPosition);  
}

/**
 * Set the standing order we're working with
 * @param theOrder The command to move over to
 */
void setStandingOrder() {
  int changed = false;

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

