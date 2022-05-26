#include "wifiConnect.hpp"
#include <HttpClient.h>

// WiFi information
char ssid[] = "Moto G (4) 7314";    
char pass[] = "93f5b38274e5"; 

// char ssid[] = "CDS-Resident";    
// char pass[] = "AC86fm!6"; 

// AWS server information
const char kHostname[] = "13.52.98.79";
int port = 5000;
const char kPath[] = "/?var=10";
const char username[] = "ubuntu";

// weather API information
const char apiHostname[] = "api.open-meteo.com";
const char apiPath[] = "/v1/forecast?latitude=33.6405&longitude=-117.8443&temperature_unit=fahrenheit&current_weather=true&daily=sunrise,sunset&timezone=America%2FLos_Angeles";

// time server information
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = -28800;  // offset btwn timezone and gmt -8
const int daylightOffset_sec = 3600;

// Number of milliseconds to wait without receiving any data before we give up
const int kNetworkTimeout = 30*1000;
// Number of milliseconds to wait if no data is available before trying again
const int kNetworkDelay = 1000;

namespace {
    void parse_iso8601(const char* time_str, tm* time_struct){
        // "2022-05-22T05:44";
        int y,M,d,h,m;
        sscanf(time_str, "%d-%d-%dT%d:%d", &y, &M, &d, &h, &m);

        time_struct->tm_hour = h;
        time_struct->tm_min = m;
        time_struct->tm_sec = 0;
    }

    tm* make_time_struct(){
        struct tm * time_struct = getCurTime();          // autofill the struct to default vals
        return time_struct;
    }
    
}

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

std::string httpRequest(const char* hostname,  const char* path, bool debug=false){
    int err =0;
  
    WiFiClient c;
    HttpClient http(c);
    std::string payload("");

    err = http.get(hostname, path);
    if (err == 0) {   
        if (debug){  Serial.println("startedRequest ok"); }
    
        err = http.responseStatusCode();
        if (err >= 0) {
             if (debug){ Serial.print("Got status code: "); Serial.println(err); }
            
            

        err = http.skipResponseHeaders();
            if (err >= 0) {
                int bodyLen = http.contentLength();

                if (debug){
                    Serial.print("Content length is: ");
                    Serial.println(bodyLen);
                    Serial.println();
                    Serial.println("Body returned follows:");
                }
                
                unsigned long timeoutStart = millis();
                while ( (http.connected() || http.available()) && ((millis() - timeoutStart) < kNetworkTimeout) && bodyLen > 0) {
                    if (http.available()) {
                        char c = http.read();

                        if (debug){
                            Serial.print(c);
                        }
                        
                        payload += c;
                        bodyLen--;
                        timeoutStart = millis();                     // We read something, reset the timeout counter
                    }
                    else  {
                        delay(kNetworkDelay);                       // We haven't got any data, so let's pause to allow some to arrive
                    }
                }
            }
            else {
                Serial.print("Failed to skip response headers: ");
                Serial.println(err);
            }
        }
        else {    
            Serial.print("Getting response failed: ");
            Serial.println(err);
        }
    }
    else {
        Serial.print("Connect failed: ");
        Serial.println(err);
    }
    http.stop();
    return payload;
}

// https://arduinojson.org/v6/assistant/#/step4
StaticJsonDocument<1536> jsonify(const std::string response){
    StaticJsonDocument<1536> doc;
    DeserializationError error = deserializeJson(doc, response);

    if (error) {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
    }
    return doc;
}

float getOutsideTemp(StaticJsonDocument<1536> doc){
    return doc["current_weather"]["temperature"]; // 62.7
}

tm* getDailySunset(StaticJsonDocument<1536> doc){
    JsonArray daily_sunset = doc["daily"]["sunset"];

    struct tm * sunset_tm = make_time_struct();
    parse_iso8601(daily_sunset[0], sunset_tm);
    return sunset_tm;
}

tm* getDailySunrise(StaticJsonDocument<1536> doc){
    JsonArray daily_sunrise = doc["daily"]["sunrise"];

    struct tm * sunrise_tm = make_time_struct();
    parse_iso8601(daily_sunrise[0], sunrise_tm);
    return sunrise_tm;
}

StaticJsonDocument<1536> requestWeatherJson() {
    std::string resp =  httpRequest(apiHostname, apiPath);
    return jsonify(resp);
 }

tm* getCurTime () {
    struct tm * t = new tm;
    configTime( gmtOffset_sec, daylightOffset_sec, ntpServer);
    if (!getLocalTime(t)){
         Serial.println("failed to obtain time");
    }
    return t;
}

