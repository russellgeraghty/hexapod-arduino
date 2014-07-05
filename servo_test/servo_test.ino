/* Sweep
 by BARRAGAN <http://barraganstudio.com> 
 This example code is in the public domain.

 modified 8 Nov 2013
 by Scott Fitzgerald
 http://arduino.cc/en/Tutorial/Sweep
*/ 

#include <Servo.h> 
 
#define FWD_BACK 2
#define SHOULDER 3
#define KNEE 4

const int min_shoulder_travel = 75;
const int max_shoulder_travel = 115;

const int min_fwd_travel = 0;
const int max_fwd_travel = 180;
 
Servo knee;
Servo shoulder;  
Servo fwd;

int pos = 0;    // variable to store the servo position 
 
void setup() 
{ 
  fwd.attach(FWD_BACK);
  shoulder.attach(SHOULDER);
  knee.attach(KNEE);
  
  knee.write(90);
  shoulder.write(min_shoulder_travel);
  fwd.write(min_fwd_travel);
} 
 
void loop() 
{ 
  for(pos = min_shoulder_travel; pos <= max_shoulder_travel; pos += 1) 
  {                                  
    shoulder.write(pos);             
    delay(15);                       
  } 
  for(pos = max_shoulder_travel; pos>=min_shoulder_travel; pos-=1)    
  {                                
    shoulder.write(pos);             
    delay(15);   
  } 
  
  for (pos = min_fwd_travel; pos <= max_fwd_travel; pos++) {
    fwd.write(pos);
    delay(15);
  }
  
  for (pos = max_fwd_travel; pos >= min_fwd_travel; pos--) {
    fwd.write(pos);
    delay(15);
  }
} 

