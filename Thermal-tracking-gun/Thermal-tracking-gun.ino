
#include <Wire.h>
#include <Adafruit_AMG88xx.h>
#include "Servo.h"

#define SERVO_PIN A2

Servo Servomotor;

Adafruit_AMG88xx amg;

void setup() {
    Serial.begin(9600);
    Serial.println("");

    Serial.println(F("AMG88xx test"));

    bool status;
    
    // default settings
    status = amg.begin();
    if (!status) {
        Serial.println("Could not find a valid AMG88xx sensor, check wiring!");
        while (1);
    }
    
    Serial.println("-- Thermistor Test --");

    Serial.println();

    delay(100); // let sensor boot up
}

void loop() {
  // put your main code here, to run repeatedly:

}
