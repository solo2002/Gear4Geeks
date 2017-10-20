// Input format
// d - toggle debug printout

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

uint8_t LSpdNew = 0;
uint8_t LDirNew = RELEASE;
uint8_t RSpdNew = 0;
uint8_t RDirNew = RELEASE;

int CntrlMode = 0; // 0 = waiting for settings; 1 = receiving settings
int CntrlSide = 0; // -1 = left motor, +1 = right motor, 0 - non-defined
int CntrlDir  = RELEASE; // direction
int CntrlSpd  = 0; // speed

int NLoops = 0;
bool Debug = false;

void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps
  Serial.println("Adafruit Motorshield v2 - DC Motor Control!");

  AFMS.begin();  // create with the default frequency 1.6KHz
  //AFMS.begin(1000);  // OR with a different frequency, say 1KHz
}

void loop() {
  NLoops++;
  while (Serial.available() < 1) {} // Do nothing until character on serial

  if (Serial.available()) {


    char cntr = Serial.read();

    switch(cntr) // Perform an action depending on the command
    {
      case 'd': // Toggle debug
        if (CntrlMode == 0) {
          if (Debug == true) {
            Debug = false;
          } else {
            Debug = true;
          }
          Serial.print("Debug set to ");
          Serial.println(Debug);
        }
        break;
      case 'p': // Print motors status
        if (CntrlMode == 0) {
          print_state();
        }
        break;
      case 's': // Set mode to receiving speed settings
        if (CntrlMode == 0) {
          CntrlMode = 1;
        }
        break;
      case 'l': // Set motor side to Left
        if (CntrlMode == 1) {
          CntrlSide = -1;
        }
        break;
      case 'r': // Set motor side to Right
        if (CntrlMode == 1) {
          CntrlSide = 1;
        }
        break;
      case 'f': // 
        if (CntrlMode == 1) {
          CntrlDir = FORWARD;
        }
        break;
      case 'b': // 
        if (CntrlMode == 1) {
          CntrlDir = BACKWARD;
        }
        break;
      case 'z': // 
        if (CntrlMode == 1) {
          CntrlDir = RELEASE;
        }
        break;
      case '0': // 
        if (CntrlMode == 1) {
          CntrlSpd = CntrlSpd*10 + 0;
        }
        break;
      case '1': // 
        if (CntrlMode == 1) {
          CntrlSpd = CntrlSpd*10 + 1;
        }
        break;
      case '2': // 
        if (CntrlMode == 1) {
          CntrlSpd = CntrlSpd*10 + 2;
        }
        break;
      case '3': // 
        if (CntrlMode == 1) {
          CntrlSpd = CntrlSpd*10 + 3;
        }
        break;
      case '4': // 
        if (CntrlMode == 1) {
          CntrlSpd = CntrlSpd*10 + 4;
        }
        break;
      case '5': // 
        if (CntrlMode == 1) {
          CntrlSpd = CntrlSpd*10 + 5;
        }
        break;
      case '6': // 
        if (CntrlMode == 1) {
          CntrlSpd = CntrlSpd*10 + 6;
        }
        break;
      case '7': // 
        if (CntrlMode == 1) {
          CntrlSpd = CntrlSpd*10 + 7;
        }
        break;
      case '8': // 
        if (CntrlMode == 1) {
          CntrlSpd = CntrlSpd*10 + 8;
        }
        break;
      case '9': // 
        if (CntrlMode == 1) {
          CntrlSpd = CntrlSpd*10 + 9;
        }
        break;
      case 'e':
        if(CntrlMode == 1) {
          if (CntrlSide == -1) {
            LDirNew = CntrlDir;
            LSpdNew = CntrlSpd;
          } else if (CntrlSide == 1) {
            RDirNew = CntrlDir;
            RSpdNew = CntrlSpd;
          }

          motorCntrl();
          
          CntrlMode = 0;
          CntrlSide = 0;
          CntrlDir  = RELEASE;
          CntrlSpd  = 0;
        }
        break;
      
      default:
        stop();
        break;
    }
  }
}

void motorCntrl() {
  bool isUpdated = false;
  
  if (LDirNew != LDir || LSpdNew != LSpd) {
    LDir = LDirNew;
    LSpd = LSpdNew;
    LMotor->setSpeed(LSpd);
    LMotor->run(LDir);
    isUpdated = true;
  }
  
  if (RDirNew != RDir || RSpdNew != RSpd) {
    RDir = RDirNew;
    RSpd = RSpdNew;
    RMotor->setSpeed(RSpd);
    RMotor->run(RDir);
    isUpdated = true;
  }
  
  if(Debug && isUpdated) {
    print_state();
  }
}

void print_state(void) {
  Serial.print(NLoops);
  Serial.print("\t");
  if (LDir == FORWARD) {
    Serial.print("F");
  } else if (LDir == BACKWARD) {
    Serial.print("B");
  } else if (LDir == RELEASE) {
    Serial.print("R");
  } else {
    Serial.print("None");
  }
  Serial.print("\t");
  Serial.print(LSpd);
  Serial.print("\t");
  if (RDir == FORWARD) {
    Serial.print("F");
  } else if (RDir == BACKWARD) {
    Serial.print("B");
  } else if (RDir == RELEASE) {
    Serial.print("R");
  } else {
    Serial.print("None");
  }
  Serial.print("\t");
  Serial.println(RSpd);
}

void stop(void) { // Stop
  LSpdNew = 0;
  LDirNew = RELEASE;
  RSpdNew = 0;
  RDirNew = RELEASE;
  motorCntrl();
  if(Debug) {
    Serial.println("Stop both motors");
  }
}
