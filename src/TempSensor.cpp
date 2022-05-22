#include "TempSensor.hpp"

TempSensor::TempSensor(float preferredTemp)
    : prefTemp{preferredTemp}
    {
        DHT.begin();
    }

float TempSensor::readIndoorTemp(){
    DHT.read();
    return DHT.getTemperature();
}

float TempSensor::getPreferredTemp(){
    return prefTemp;
}

void TempSensor::setPreferredTemp(float newTemp){
    prefTemp = newTemp;
}


