#include <Adafruit_AMG88xx.h>

/* Sweep
 by BARRAGAN <http://barraganstudio.com>
 This example code is in the public domain.

 modified 8 Nov 2013
 by Scott Fitzgerald
 http://www.arduino.cc/en/Tutorial/Sweep
*/
#include <Wire.h>
#include <Servo.h>

#define alexa       12
//#define trigger     8
const int trigger = 8; 

Adafruit_AMG88xx amg;
float pixels[AMG88xx_PIXEL_ARRAY_SIZE];

Servo rotate;  // create servo object to control a servo
Servo updown;

// twelve servo objects can be created on most boards

int pos = 0;    // variable to store the servo position

void setup() {
  
    rotate.attach(11);  // attaches the servo on pin 9 to the servo object
    updown.attach(10);
    amg.begin();
    delay(3000);
    pinMode(trigger,OUTPUT);        //put it to the bottom of the setup to avoid FIRE in boot-up sequence
    pinMode(alexa, INPUT);
}

void loop() {
    if (digitalRead(alexa) == HIGH) {
        Serial.println("ALXA mode");
    }
    
    int count = 70;
    for (pos = 45; pos <= 90; pos += 2) { // goes from 0 degrees to 180 degrees
      // in steps of 1 degree
      count+= 2;
      updown.write(count);
      rotate.write(pos);              // tell servo to go to position in variable 'pos'
      if(sense){
        pulltrigger();
      }
      delay(60);                       // waits 15ms for the servo to reach the position
    }
    for (pos = 90; pos <= 135; pos += 2) { // goes from 0 degrees to 180 degrees
      // in steps of 1 degree
      count -+ 1;
      updown.write(count);
      rotate.write(pos);     
      if(sense){
        pulltrigger();
      }
      delay(60);                       // waits 15ms for the servo to reach the position
    }
    for (pos = 135; pos >= 90; pos -= 2) { 
      count -= 1;                       // goes from 180 degrees to 0 degrees
      updown.write(count);
      rotate.write(pos);              // tell servo to go to position in variable 'pos'
      if(sense){
        pulltrigger();
      }
      delay(60);                       // waits 15ms for the servo to reach the position
    }
    for (pos = 90; pos >= 45; pos -= 2) { // goes from 180 degrees to 0 degrees
      count += 2;
      updown.write(count);
      rotate.write(pos);              // tell servo to go to position in variable 'pos'
      if(sense){
        pulltrigger();
      }
      delay(60);                       // waits 15ms for the servo to reach the position
    }
}
boolean sense_old(){
    amg.readPixels(pixels);
    
    if(pixels[37] > 26 || pixels[36] > 26 || pixels[29] > 26 || pixels[28] > 26){
      return true;
    }
    return false;

}

char sense() {
    amg.readPixels(pixels);
      
      if(pixels[37] > 26 || pixels[36] > 26 || pixels[29] > 26 || pixels[28] > 26){
        return 'm';
      } else if (pixels[33] > 26 || pixels[25] > 26 ) {
        return 'l';
      } else if (pixels[4] > 26 || pixels[5] > 26 ) {
        return 'r';
      } else if (pixels[60] > 26 || pixels[61] > 26 ) {
        return 'u';
      }
      
      return 'p';
}

// void pulltrigger(){
//     while(sense()){
//       digitalWrite(trigger, LOW);
//       delay(1000);
//     }
//     digitalWrite(trigger, HIGH);
// }

void pulltrigger(){
    while(sense() == 'm' || sense() == 'u' || sense() == 'r' || sense() == 'l'){
      digitalWrite(trigger, LOW);
      delay(1000);
    }
    digitalWrite(trigger, HIGH);
}
