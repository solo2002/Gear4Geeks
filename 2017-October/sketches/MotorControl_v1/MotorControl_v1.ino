#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 

// Select which 'port' M1, M2, M3 or M4.
Adafruit_DCMotor *LMotor = AFMS.getMotor(3); // Left Motor
Adafruit_DCMotor *RMotor = AFMS.getMotor(4); // Right Motor

uint8_t LSpd = 0; // Speed to Left Motor (0 - 255)
uint8_t LDir = RELEASE; // State of Left Motor (one of 3 values):
                        //  * FORWARD  - Rotate in a forward direction
                        //  * BACKWARD - Rotate in the reverse direction
                        //  * RELEASE  - Stop rotation
uint8_t RSpd = 0; // Power to Right Motor
uint8_t RDir = RELEASE; // State of Right Motor

void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps
  Serial.println("Adafruit Motorshield v2 - DC Motor Control!");

  AFMS.begin();  // create with the default frequency 1.6KHz
  //AFMS.begin(1000);  // OR with a different frequency, say 1KHz
}

void loop() {
  while (Serial.available() < 1) {} // Do nothing until character on serial

  if (Serial.available()) {
  
    char cntr = Serial.read();
    uint8_t lSpd = 100;
    uint8_t rSpd = 100;
    uint8_t lDir = RELEASE;
    uint8_t rDir = RELEASE;

    switch(cntr) // Perform an action depending on the command
    {
      case 'w': // Move Forward
        forward (lSpd,rSpd);
        break;
      case 's': // Move Backwardsw
        backward (lSpd,rSpd);
        break;
      case 'a': // Turn Left
        left (lSpd,rSpd);
        break;
      case 'd': // Turn Right
        right (lSpd,rSpd);
        break;
      default:
        stop();
        break;
    }
  }
}

void stop(void) { // Stop
  LMotor->setSpeed(0);
  LMotor->run(RELEASE);
  RMotor->setSpeed(0);
  RMotor->run(RELEASE);
  Serial.println("Stop");
}

void forward(uint8_t lSpd, uint8_t rSpd) { // Forward
  LMotor->setSpeed(100);
  LMotor->run(FORWARD);
  RMotor->setSpeed(100);
  RMotor->run(FORWARD);
  Serial.println("Forward");
}

void backward(uint8_t lSpd, uint8_t rSpd) { // Backward
  LMotor->setSpeed(100);
  LMotor->run(BACKWARD);
  RMotor->setSpeed(100);
  RMotor->run(BACKWARD);
  Serial.println("Backward");
}

void left(uint8_t lSpd, uint8_t rSpd) { // Left
  LMotor->setSpeed(0);
  LMotor->run(FORWARD);
  RMotor->setSpeed(100);
  RMotor->run(FORWARD);
  Serial.println("Left");
}

void right(uint8_t lSpd, uint8_t rSpd) { // Right
  LMotor->setSpeed(100);
  LMotor->run(FORWARD);
  RMotor->setSpeed(0);
  RMotor->run(FORWARD);
  Serial.println("Left");
}
