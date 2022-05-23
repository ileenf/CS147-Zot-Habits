#ifndef MOTION_HPP
#define MOTION_HPP

#include <Arduino.h>
#include <Wire.h>

class Motion {
    public:
        Motion(const int motionSensorPin);
    
    private:
        void IRAM_ATTR detectsMovement();
        int motionSensorPin;
};

#endif