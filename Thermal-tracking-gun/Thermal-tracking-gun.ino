unsigned int yp = 0;
unsigned int yn = 0;
unsigned int i = 0;         //index



#include <Wire.h>
#include <Adafruit_AMG88xx.h>
#include "Servo.h"

#define SERVO_X     A2              //horizonal servo
#define SERVO_Y     A3              //vertical servo
Servo servoMotor_X, servoMotor_Y;

Adafruit_AMG88xx amg;

//INT pin from the sensor board goes to this pin on your microcontroller board
#define INT_PIN 3

//interrupt levels (in degrees C)
//any reading on any pixel above TEMP_INT_HIGH degrees C, or under TEMP_INT_LOW degrees C will trigger an interrupt
//30 degrees should trigger when you wave your hand in front of the sensor
#define TEMP_INT_HIGH 30
#define TEMP_INT_LOW 15

volatile bool intReceived = false;
uint8_t pixelInts[8];

void setup() {
    Serial.begin(9600);
    Serial.println("");

    Serial.println(F("AMG88xx test"));

    servo_init();
    thermal_sensor_init();
    thermal_sensor_interrupt_init();
    
}

void loop() {
      if(intReceived){
            //get which pixels triggered
            amg.getInterrupt(pixelInts);

            /* ------- Algorithm to calculate the sum of bottom half and top half */
            yp = 0;
            yn = 0;
            for(i = 0; i < 4; i++){
                yp += pixelInts[i];
            }
            for(i = 4; i<8; i++){
                yn += pixelInts[i];
            }
            Serial.println("**** interrupt received! ****");
            for(int i=0; i<8; i++){
              Serial.println(pixelInts[i], BIN);
            }
            if(yn <  yp){
               Serial. println("move up");
            }
            else
            {
                Serial.println("move down");
            }
            Serial.println();

            /* ------ Finished Calculation -------- */
            
            //clear the interrupt so we can get the next one!
            amg.clearInterrupt();
            
            intReceived = false;
      }

}

void thermal_sensor_init() {
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

void thermal_sensor_interrupt_init() {
      amg.setInterruptLevels(TEMP_INT_HIGH, TEMP_INT_LOW);

      //set to absolue value mode
      amg.setInterruptMode(AMG88xx_ABSOLUTE_VALUE);
    
      //enable interrupts
      amg.enableInterrupt();
    
      //attach to our Interrupt Service Routine (ISR)
      attachInterrupt(digitalPinToInterrupt(INT_PIN), AMG88xx_ISR, FALLING);

      Serial.println("INTERRUPT setup finished");
}

void servo_init() {
    servoMotor_X.attach(SERVO_X);
    servoMotor_X.write(90);
    servoMotor_Y.attach(SERVO_Y);
    servoMotor_Y.write(90);
}

/* INTERRUPT activity: KEEP IT SHORT to avoid CPU overhead */
void AMG88xx_ISR() {
    intReceived = true;
}
