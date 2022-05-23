#ifndef TEMPSENSOR_HPP
#define TEMPSENSOR_HPP

#include "DHT20.h"


class TempSensor
{
public:
    // initialize with a preferred temp
    TempSensor(float preferredTemp);
    // specify units
    TempSensor(float preferredTemp, bool inF);
    
    void setPreferredTemp(float newTemp);  // user can alter indoor temp pref
    float getPreferredTemp();
    float readIndoorTemp();
    bool inFahrenheit();

private:
    float prefTemp; // stored in Celcius
    bool f;  
    DHT20 DHT;

    
};



#endif