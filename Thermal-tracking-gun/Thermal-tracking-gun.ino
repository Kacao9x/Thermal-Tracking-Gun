/* variables */
unsigned int y_P = 0;
unsigned int y_N = 0;
unsigned int x_L = 0;
unsigned int x_R = 0;

/* variable for moving servos */
int xPos = 90;
int yPos = 90;

#define x_min 45
#define x_max 135

#define y_min 25
#define y_max 115

unsigned int i = 0, j = 0;         //index
char dir[2];

bool FIRE = false;
unsigned long fireDelay = 0;
unsigned long searchDelay = 0;

#include <Wire.h>
#include <Adafruit_AMG88xx.h>
#include "Servo.h"

/* variables for controlling servo */
#define SERVO_X     11              //horizonal servo
#define SERVO_Y     10              //vertical servo
Servo servoMotor_X, servoMotor_Y;

/* variables for controlling Trigger (FIRE) of the gun */
#define FIRE_BUTTON     8           //pull the trigger
#define FIRE_MOTOR      9           //spin the motor while searching the target

/* variables for thermal sensor */
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

void servo_init();
void thermal_sensor_init();
void thermal_sensor_interrupt_init();
void clearVar();
void moveGun();
void check_bottom_and_top_half();
void check_left_right_half_with_fire();
void chaseTarget();

void setup() {
    Serial.begin(9600);
    Serial.println("");

    Serial.println(F("AMG88xx test"));

    servo_init();
    thermal_sensor_init();
    thermal_sensor_interrupt_init();
    delay(1000);                                //let the sensor caliberate the room temp

    pinMode(FIRE_BUTTON, OUTPUT);               //set FIRE_BUTTON pin as output
    pinMode(FIRE_MOTOR, OUTPUT);
    
}


void loop() {
    /* reset variable */
    clearVar();
    
    if (intReceived) {
        //searchDelay = millis();

        //get which pixels triggered
        amg.getInterrupt(pixelInts);
        Serial.println("**** interrupt received! ****");

        /* ------- VERTICAL: Algorithm to calculate the sum of bottom half and top half ----- */
        check_bottom_and_top_half();
        /* ------ Finished Vertial Calculation -------- */

        /* ------- HORIZONTAL: Algorithm to calculate the sum of left half and right half ------- */
        check_left_right_half_with_fire();        
        /* ------ Finished Horizontal Calculation -------- */
        
        
        //Start shooting
        Serial.println(FIRE);
        if (FIRE == true) {
            
            while (intReceived) {
                startFire();
                delay(500);
                stopFire();
            }
            stopFire();
            fireDelay = millis();
        } else {
            chaseTarget();

            if( millis - fireDelay > 3000) {
                stopFireMotor();
                FIRE = false;
            }
        }

        //moveGun();
        //chaseTarget();


        //clear the interrupt so we can get the next one!
        amg.clearInterrupt();       // clear Interrupt
        intReceived = false;        // reset the interrupt flag
         
    } /* ------ END of Interrupt trigger received -------- */

    if(millis() - fireDelay > 1000){
        //digitalWrite(FIRE_BUTTON, HIGH);
        stopFireMotor();

    }

    // if(millis() - searchDelay > 1000){
    //   // start searching 
    // }

} /* ------ END of MAIN LOOP -------- */


void check_bottom_and_top_half() {
    for(i = 0; i < 4; i++) {
        y_P += pixelInts[i];
    }
    for(i = 4; i<8; i++) {
        y_N += pixelInts[i];
    }
}

void check_left_right_half_with_fire(){
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
                    startFireMotor();
                    break;
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
                    startFireMotor();
                    break;
                }
                value /= 2;
            } else {
                value /= 2;
            }
        }/* --- END of the left 4-bit interation --- */
    }   /* --- END of the row interation --- */

}


void gun_fire(unsigned int pinNumber) {
    digitalWrite (pinNumber, HIGH);   //Need caliberation: how long to set the PIN high to fire?
    delay (500);                        //CAUTION: system_delay --> use timer here
    digitalWrite (pinNumber, LOW);
    delay (100);
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

void clearVar(){
    x_L = 0;
    x_R = 0;
    y_N = 0;
    y_P = 0;
}

/* !!!!!!!!! Need to check how fast the servo can move */
void chaseTarget() {
    /* move horizontally */
    if (x_L > x_R && x_L <= x_max ) {
        Serial.println("move Left");
        xPos +=1;
        servoMotor_X.write(xPos);
        //move_gun('R');
    } else if ( x_L < x_R && x_R >= x_min ) {
        Serial.println("move Right");
        xPos -= 1;
        servoMotor_X.write(xPos);
        //move_gun('L');
    }

    /* move vertically */
    if (y_P >  y_N && y_P >= y_min ) {
        Serial. println("move down");
        yPos -= 1;
        servoMotor_Y.write(yPos);
        //move_gun('D');
    } else if (y_P <  y_N && y_P <= y_max ){
        Serial.println("move UP");
        yPos += 1;
        servoMotor_Y.write(yPos);
        //move_gun('D');
    }
}

void moveGun(){
  if(x_L > x_R){
    if(x_L <= x_max){
      xPos += 1;
      servoMotor_X.write(xPos);
    }
  }
  else if(x_L < x_R){
    if(x_R >= x_min){
      xPos -= 1;
      servoMotor_X.write(xPos);
    }
  }
  if(y_P > y_N){
    if(y_P >= y_min){
      yPos -= 1;
      servoMotor_Y.write(yPos);
    }
  }
  else if(y_P < y_N){
    if(y_N <= y_max){
      yPos += 1;
      servoMotor_Y.write(yPos);
    }
  }
}


void move_gun(const char* dir) {
    switch (*dir) {
        case 'L':
            //move left: servoMotor_X.write(180);       //------- increment 5 degree at a times
            Serial.println("move left");
            break;
        case 'R':
            //move right: servoMotor_X.write(0);
            Serial.println("move right");
            break;
        case 'U':
            //move up: servoMotor_Y.write(0);
            Serial.println("move up");
            break;
        case 'D':
            //move down: servoMotor_Y.write(180);
            Serial.println("move down");
            break;
        default:
            break;
    }
}

void startFireMotor() {
    digitalWrite(FIRE_MOTOR, HIGH);
}
void stopFireMotor() {
    digitalWrite(FIRE_MOTOR, LOW);
}
void startFire () {
    digitalWrite(FIRE_BUTTON, HIGH);
}
void stopFire () {
    digitalWrite(FIRE_BUTTON, LOW);
}


/* INTERRUPT activity: KEEP IT SHORT to avoid CPU overhead */
void AMG88xx_ISR() {
    intReceived = true;
}