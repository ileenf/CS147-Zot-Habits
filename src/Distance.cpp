#include "Distance.hpp"

Distance::Distance(const int trigPin, const int echoPin)
    : trigPin{trigPin}, echoPin{echoPin} {
}

void Distance::initializeDistanceThreshold(float threshold) {
    distanceThreshold = threshold;
}

void Distance::setDistanceInch() {
    long duration = pulseIn(echoPin, HIGH);
    float distanceCm = duration * SOUND_SPEED/2;
    distanceInch = distanceCm * CM_TO_INCH;
}

float Distance::getDistanceInch() const {
    return distanceInch;
}

bool Distance::distanceGreaterThanThreshold() {
    return distanceInch > distanceThreshold;
}
