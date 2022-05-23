#include "Motion.hpp"

Motion::Motion(const int motionSensorPin)
    : motionSensorPin{motionSensorPin} {
}

void IRAM_ATTR Motion::detectsMovement(){
  Serial.println("MOTION DETECTED!!!");
}