#ifndef WIFICONNECT_HPP
#define WIFICONNECT_HPP

#include <WiFi.h>
#include <HttpClient.h>

void initializeWifi(const char* ssid, const char* pass);
void httpRequest(const char* hostname, const char* path);



#endif

