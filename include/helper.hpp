#ifndef HELPER_HPP
#define HELPER_HPP

#include <Arduino.h>
#include "time.h"

const int DO_NOTHING = 0;
const int OPEN_WINDOW = 1;
const int CLOSE_WINDOW = 2;

void printWeatherData(float outside_temp, tm* sunrise_tm, tm* sunset_tm);
void printCurTime(tm* cur_tm);
void printIndoorTemp(float temp);
void printOutdoorTemp(float temp);
void printRec(int rec);


int giveRec(float indoor, float outdoor, float pref_temp);

#endif
