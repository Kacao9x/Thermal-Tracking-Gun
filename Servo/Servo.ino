#include "Servo.h"

#define SERVO_PIN 7

Servo servoMotor;

int angle_degree = 0;               // angle to move
int angle_10bit_readout = 0;
int thermal_readout = 0;          // temperature value readout from sensor


void setup() {
    Serial.begin( 9600 );
    Serial.println("");

    Serial.println(F("Test servo PWM control"));
    servoMotor.attach(SERVO_PIN);                       // signal line: pin D7
    servoMotor.write(90);                                       // turn to 90 degree
}

void loop() {
    //testServo();

    /* read value from sensor */
    //angle = maxValue();
    angle_10bit_readout = 511;                          //dummy value
    angle_degree = map(angle_10bit_readout, 0, 1023, 0, 179);
    servoMotor.write(angle_degree);
    
}

void testServo() {
     servoMotor.write(0);       // Rotate Servo to 0 Degrees
     delay(500);                        // Delay to allow Servo time to Move
     servoMotor.write(90);      // Rotate Servo to 90 Degrees
     delay(500);                        // Delay to allow Servo time to Move
     servoMotor.write(180);     // Rotate Servo to 180 Degrees
     delay(500);                      // Delay to allow Servo time to Move
}

