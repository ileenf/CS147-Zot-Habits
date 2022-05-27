#ifndef WIFICONNECT_HPP
#define WIFICONNECT_HPP

// 1536 recommended buffer size https://arduinojson.org/v6/assistant
# define BUF_SIZE 1536

#include <string>
#include <WiFi.h>
#include <HttpClient.h>
#include <ArduinoJSON.h>
#include "time.h"

void initializeWifi();
std::string httpRequest(const char* hostname, const char* path, bool debug);
StaticJsonDocument<BUF_SIZE> jsonify(const std::string response); 

StaticJsonDocument<1536> requestWeatherJson();
float getOutsideTemp(StaticJsonDocument<1536> doc);
tm* getDailySunset(StaticJsonDocument<1536> doc);
tm* getDailySunrise(StaticJsonDocument<1536> doc);
tm* getCurTime ();
void sendSittingData(int numExceededSittingTime);
void sendData(char* pathName, int temp);

#endif

