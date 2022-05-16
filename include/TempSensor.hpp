#ifndef TEMPSENSOR_HPP
#define TEMPSENSOR_HPP

#include "DHT20.h"

class TempSensor
{
public:
    // initialize with a preferred temp
    TempSensor(float preferredTemp);
    
    void setPreferredTemp(float newTemp);  // user can alter indoor temp pref
    // int giveRec();


private:

    float prefTemp;
    DHT20 DHT;

    float readIndoorTemp();
};



#endif