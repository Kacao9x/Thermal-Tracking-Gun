/* variables */
unsigned int y_P = 0;
unsigned int y_N = 0;
unsigned int x_L = 0;
unsigned int x_R = 0;

/* variable for moving servos */
int xPos = 0;
int yPos = 0;



unsigned int i = 0, j = 0;         //index
char dir[2];

bool FIRE = false;

#define x_max       135
#define x_min       45

#define y_max
#define y_min

#include <Wire.h>
#include <Adafruit_AMG88xx.h>
#include "Servo.h"

/* variables for controlling servo */
#define SERVO_X     A2              //horizonal servo
#define SERVO_Y     A3              //vertical servo
Servo servoMotor_X, servoMotor_Y;

/* variables for controlling Trigger (FIRE) of the gun */
#define FIRE_BUTTON     10

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

void setup() {
    Serial.begin(9600);
    Serial.println("");

    Serial.println(F("AMG88xx test"));

    servo_init();
    thermal_sensor_init();
    thermal_sensor_interrupt_init();
    delay(1000);                                //let the sensor caliberate the room temp

    pinMode(FIRE_BUTTON, OUTPUT);
    
}


void loop() {
    /* reset variable */
    clearVar();
    
    if(intReceived){
        //get which pixels triggered
        amg.getInterrupt(pixelInts);
        Serial.println("**** interrupt received! ****");

        /* ------- VERTICAL: Algorithm to calculate the sum of bottom half and top half ----- */
        check_bottom_and_top_half();
        /* ------ Finished Vertial Calculation -------- */

        /* ------- HORIZONTAL: Algorithm to calculate the sum of left half and right half ------- */
        check_left_right_half_with_fire();        
        /* ------ Finished Horizontal Calculation -------- */
        
        if (!FIRE) {
            comparePositionValue();
        }

        //clear the interrupt so we can get the next one!
        amg.clearInterrupt();       // clear Interrupt
        intReceived = false;        // reset the interrupt flag


        // if(FIRE == true){
        //     digitalWrite(10, HIGH);
        // } else  {
        //     digitalWrite(10, LOW);
        // }


    }
    
    

}

void comparePositionValue() {
    /* move vertically */
    if(y_N <  y_P) {
        Serial. println("move up");
        move_gun('U');
        // TO-DO: move the servo
    } else {
        Serial.println("move down");
        move_gun('D');
        // TO-DO: move the servo
    }

    /* move horizontally */
    if (x_L < x_R) {
        Serial.println("move Right");
        move_gun('R');
        //move right;
    } else {
        Serial.println("move Left");
        move_gun('L');
        //move left;
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
       /*case "RU":
           //move right: servoMotor_X.write(0);
           //move up: servoMotor_Y.write(0);
           Serial.println("RIGHT-UP");
           break;
       case "RD":
           //move right: servoMotor_X.write(0);
           //move down: servoMotor_Y.write(180);
           Serial.println("RIGHT-DOWN");
           break;
       case "LU":
           //move left: servoMotor_X.write(180);
           //move up: servoMotor_Y.write(0);
           Serial.println("LEFT-UP");
           break;
       case "LD":
           //move left: servoMotor_X.write(180);
           //move down: servoMotor_Y.write(180);
           Serial.println("LEFT-DOWN");
           break; */
        default:
            break;
    }
}


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
                    //FIRE = true;
                    gun_fire(FIRE_BUTTON);
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
                    // FIRE = true;
                    gun_fire(FIRE_BUTTON);
                }
                value /= 2;
            } else {
                value /= 2;
            }
        }/* --- END of the left 4-bit interation --- */
    }   /* --- END of the row interation --- */

    /* FIRE test on serial monitor*/
//     Serial.println();
//         if (FIRE == true ) {
//             Serial.println("FIREEEEE");
//             gun_fire(FIRE_BUTTON);
//         } else {
//             Serial.println("Checkkking target");
//         }
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

/* INTERRUPT activity: KEEP IT SHORT to avoid CPU overhead */
void AMG88xx_ISR() {
    intReceived = true;
}

void clearVar(){
    x_L = 0;
    x_R = 0;
    y_N = 0;
    y_P = 0;

    FIRE = false;
}

void checTrigger() {
    
}

void moveFireUD() {
    if(y_N <  y_P) {
        move_gun('U');
        // TO-DO: move the servo
    } else {
        move_gun('D');
        // TO-DO: move the servo
    }
}

void moveFireLR() {
    if (x_L < x_R) {
        //Serial.println("move Right");
        *dir = "RD";
        move_gun(dir);
    } else {
        //Serial.println("move Left");
        *dir = "LU";
        move_gun(dir);
    }
}
