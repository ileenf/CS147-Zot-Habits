#ifndef HELPER_HPP
#define HELPER_HPP

#include <Arduino.h>
#include "time.h"

const int DO_NOTHING = 0;
const int OPEN_WINDOW = 1;
const int CLOSE_WINDOW = 2;

void print_weather_data(float outside_temp, tm* sunrise_tm, tm* sunset_tm);
void print_cur_time(tm* cur_tm);
void print_indoor_temp(float temp);
void print_outdoor_temp(float temp);
void printRec(int rec);


int giveRec(float indoor, float outdoor, float pref_temp);

#endif
