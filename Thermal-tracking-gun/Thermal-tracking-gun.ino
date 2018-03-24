unsigned int y_p = 0;
unsigned int y_n = 0;
unsigned int x_L = 0;
unsigned int x_R = 0;
unsigned int i = 0, j = 0;         //index

bool FIRE = false;


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
#define TEMP_INT_HIGH 26
#define TEMP_INT_LOW 10

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
    FIRE = false;
    y_p = 0;
    y_n = 0;
    
    if(intReceived){
        //get which pixels triggered
        amg.getInterrupt(pixelInts);

        Serial.println("**** interrupt received! ****");

        /* ------- VERTICAL: Algorithm to calculate the sum of bottom half and top half */
        //y_p = 0;
        //y_n = 0;
        sum_bottom_and_top_half();
        if(y_n <  y_p) {
            Serial. println("move up");
            // TO-DO: move the servo
        } else {
            Serial.println("move down");
            // TO-DO: move the servo
        }

        /* ------ Finished Calculation -------- */


        /* ------- HORIZONTAL: Algorithm to calculate the sum of left half and right half */
        x_L = 0;
        x_R = 0;
        int value = 0;
        
        for(i=0; i<8; i++) {
            value = pixelInts[i];
            //Serial.print("value of 8bit array: "); Serial.println(value);       //print out octal value

            /* calculate the right 4 bits */
            for (j=0; j<4; j++) {
                if (value % 2 == 1) {
                    x_R += 1;
                    /* check if the central bit value is 1 */
                    if ( (j == 3 && i == 3) || (j == 3 && i == 4) ) {
                        FIRE = true;
                    }
                    //value >> 1; // divide by 2: not working b/c value is int value
                    value /= 2;
                } else {
                    value /= 2;
                }
            } /* --- END of the right 4-bit interation --- */

            /* calculate the left 4 bits */
            for (j=0; j<4; j++) {
                if (value % 2 == 1) {
                    x_L +=1;
                    /* check if the central bit value is 1 */
                    if( (j == 0 && i == 3) || (j == 0 && i == 4) ) {
                        FIRE = true;
                    }
                    value /= 2;
                } else {
                    value /= 2;
                }
            }/* --- END of the left 4-bit interation --- */
        }   /* --- END of the row interation --- */

            if (x_L < x_R) {
                Serial.println("move Right");
                //move right;
            } else {
                Serial.println("move Left");
                //move left;
            }
        
        Serial.println();
        if (FIRE == true ) {
            Serial.println("FIREEEEE");
            //gun_fire();
        } else {
            Serial.println("Checkkking tartget");
        }
        
        Serial.println();

        /* ------ Finished Calculation -------- */
        
        //clear the interrupt so we can get the next one!
        amg.clearInterrupt();       // clear Interrupt
        intReceived = false;        // reset the interrupt flag
    }

}

void sum_bottom_and_top_half() {
//            y_p = 0;
//            y_n = 0;
    for(i = 0; i < 4; i++) {
        y_p += pixelInts[i];
    }
    for(i = 4; i<8; i++) {
        y_n += pixelInts[i];
    }
}

void gun_fire() {

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
