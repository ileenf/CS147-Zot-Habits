#include "TempSensor.hpp"

namespace {
    float toFahrenheit(float tempCelcius){
        return (tempCelcius * (9/5)) + 32;
    }
    float toCelcius(float tempF){
        return (tempF - 32) * (5/9 );
    }
}

TempSensor::TempSensor(float preferredTemp)
    : prefTemp{preferredTemp}, f{false}
    {
        DHT.begin();
    }

TempSensor::TempSensor(float preferredTemp, bool inF)
    : prefTemp{preferredTemp}, f{inF}
    {
        DHT.begin();
    }

float TempSensor::readIndoorTemp(){
    DHT.read();
    float inCelcius = DHT.getTemperature();
    if (f){
        return toFahrenheit(inCelcius);
    }
    return inCelcius;
}

float TempSensor::getPreferredTemp(){
    if (f){
        return toFahrenheit(prefTemp);
    }
    return prefTemp;
}

void TempSensor::setPreferredTemp(float newTemp){
    if (f){
        prefTemp = toCelcius(newTemp);
    }
    else {
        prefTemp = newTemp;
    }
}

bool TempSensor::inFahrenheit(){
    return f;
}
