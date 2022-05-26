#ifndef HELPER_HPP
#define HELPER_HPP

#include <Arduino.h>
#include "time.h"

const int DO_NOTHING = 0;
const int OPEN_WINDOW = 1;
const int CLOSE_WINDOW = 2;
const int OPEN_BLINDS = 3;
const int TURN_ON_LIGHT = 4;
const int LIGHT_THRESHOLD = 100;

void printWeatherData(float outsideTemp, tm* sunriseTm, tm* sunsetTm);
void printCurTime(tm* curTm);
void printIndoorTemp(float temp);
void printOutdoorTemp(float temp);
void printTempRec(int rec);
void printLightRec(int rec);
void printIndoorLight(int lightVal);

bool isDaytime(tm* curTm, tm* sunset, tm* sunrise);
bool isDarkIndoors(float curLightVal);
int tmCompare(tm* t1, tm* t2);

int giveTempRec(float indoor, float outdoor, float prefTemp);
int giveLightRec(tm* curTm, tm* sunset, tm* sunrise, float curLightVal);


#endif
