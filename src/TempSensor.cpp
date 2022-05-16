#include "TempSensor.hpp"

// const int DO_NOTHING = 0;
// const int OPEN_WINDOW = 1;
// const int CLOSE_WINDOW = 2;


TempSensor::TempSensor(float preferredTemp)
    : prefTemp{preferredTemp}
    {
        DHT.begin();
    }

float TempSensor::readIndoorTemp(){
    DHT.read();
    return DHT.getTemperature();
}

void TempSensor::setPreferredTemp(float newTemp){
    prefTemp = newTemp;
}

// int TempSensor::giveRec(){
//     float outdoor = readOutdoorTemp();
//     float indoor = readIndoorTemp();


//     if (outdoor > indoor && indoor < prefTemp){
//         // OPEN WINDOW TO WARM UP HOUSE b/c it's too cold inside
//         return OPEN_WINDOW;
//     }
//     if (outdoor > indoor && indoor >= prefTemp){
//         // CLOSE WINDOW, TURN ON AC b/c too hot outside
//         return CLOSE_WINDOW;
//     }
//     if (outdoor < indoor && indoor > prefTemp){
//         // OPEN WINDOW TO COOL DOWN HOUSE b/c it's too warm inside
//         return OPEN_WINDOW;
//     }
//     if (outdoor < indoor && indoor <= prefTemp){
//         // CLOSE WINDOW, TURN ON HEATER b/c it's too cold outside
//         return CLOSE_WINDOW;
//     }

//     // if outdoor == indoor
//     return DO_NOTHING;
    
    
// }   
