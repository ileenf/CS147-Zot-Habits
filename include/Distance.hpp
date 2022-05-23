#ifndef DISTANCE_HPP
#define DISTANCE_HPP

#include <Arduino.h>
#include <Wire.h>

class Distance {
    public:
        Distance(const int trigPin, const int echoPin);
        void setDistanceInch(long duration);
        float getDistanceInch() const;
        bool distanceGreaterThanThreshold();
        void initializeDistanceThreshold(float threshold);
        float getDistanceThreshold();

    private:
        float distanceThreshold;
        float distanceInch;
        float SOUND_SPEED = 0.034;
        float CM_TO_INCH = 0.393701;
        int trigPin;
        int echoPin;

};

#endif