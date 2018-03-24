#include "Servo.h"

#define SERVO_PIN 7

Servo servoMotor;


void setup() {
    Serial.begin( 9600 );
    Serial.println("");

    Serial.println(F("Test servo PWM control"));
    servoMotor.attach(SERVO_PIN);
}

void loop() {
     servoMotor.write(0);       // Rotate Servo to 0 Degrees
     delay(500);                        // Delay to allow Servo time to Move
     servoMotor.write(90);      // Rotate Servo to 90 Degrees
     delay(500);                        // Delay to allow Servo time to Move
     servoMotor.write(180);     // Rotate Servo to 180 Degrees
     delay(500);                      // Delay to allow Servo time to Move

}
