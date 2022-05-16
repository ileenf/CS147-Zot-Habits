#include "wifiConnect.hpp"
#include <HttpClient.h>

// WiFi information
char ssid[] = "Moto G (4) 7314";    
char pass[] = "93f5b38274e5"; 

// AWS server information
const char kHostname[] = "13.52.98.79";
int port = 5000;
const char kPath[] = "/?var=10";
const char username[] = "ubuntu";

// weather API information
const char apiHostname[] = "api.open-meteo.com";
const char apiPath[] = "/v1/forecast?latitude=33.6405&longitude=-117.8443&temperature_unit=fahrenheit&current_weather=true&daily=sunrise,sunset&timezone=America%2FLos_Angeles";

// Number of milliseconds to wait without receiving any data before we give up
const int kNetworkTimeout = 30*1000;
// Number of milliseconds to wait if no data is available before trying again
const int kNetworkDelay = 1000;

void initializeWifi(){
  
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, pass);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.println("MAC address: ");
    Serial.println(WiFi.macAddress());
}

void getWeatherData() {
    httpRequest(apiHostname, apiPath);
}

void httpRequest(const char* hostname,  const char* path){
    int err =0;
  
    WiFiClient c;
    HttpClient http(c);

    err = http.get(hostname, path);
    if (err == 0)
    {
        Serial.println("startedRequest ok");

        err = http.responseStatusCode();
        if (err >= 0)
        {
        Serial.print("Got status code: ");
        Serial.println(err);

        // Usually you'd check that the response code is 200 or a
        // similar "success" code (200-299) before carrying on,
        // but we'll print out whatever response we get

        err = http.skipResponseHeaders();
        if (err >= 0)
        {
            int bodyLen = http.contentLength();
            Serial.print("Content length is: ");
            Serial.println(bodyLen);
            Serial.println();
            Serial.println("Body returned follows:");
        
            // Now we've got to the body, so we can print it out
            unsigned long timeoutStart = millis();
            char c;
            // Whilst we haven't timed out & haven't reached the end of the body
            while ( (http.connected() || http.available()) &&
                ((millis() - timeoutStart) < kNetworkTimeout) )
            {
                if (http.available())
                {
                    c = http.read();
                    // Print out this character
                    Serial.print(c);
                
                    bodyLen--;
                    // We read something, reset the timeout counter
                    timeoutStart = millis();
                }
                else
                {
                    // We haven't got any data, so let's pause to allow some to
                    // arrive
                    delay(kNetworkDelay);
                }
            }
        }
        else
        {
            Serial.print("Failed to skip response headers: ");
            Serial.println(err);
        }
        }
        else
        {    
        Serial.print("Getting response failed: ");
        Serial.println(err);
        }
    }
    else
    {
        Serial.print("Connect failed: ");
        Serial.println(err);
    }
    http.stop();
}