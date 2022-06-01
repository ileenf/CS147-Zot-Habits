#include "wifiConnect.hpp"

// WiFi information
char ssid[] = "Moto G (4) 7314";    
char pass[] = "93f5b38274e5"; 

// char ssid[] = "CDS-Resident";    
// char pass[] = "AC86fm!6"; 

// AWS server information
const char AWS_TEMPLIGHT_ROUTE[] = "http://54.177.36.79:5000/temp-light-sensor";
const char AWS_EXCEEDSITTING_ROUTE[] = "http://54.177.36.79:5000/num-exceeded-sitting";
const char AWS_SITDUR_ROUTE[] = "http://54.177.36.79:5000/sitting-duration";
const char AWS_DARKINDOORS_ROUTE[] = "http://54.177.36.79:5000/dark-indoors";
const char AWS_HOSTNAME[] = "54.177.36.79";
const int AWS_PORT = 5000;
const char AWS_USERNAME[] = "ubuntu";

// weather API information
const char API_URL[] = "https://api.open-meteo.com/v1/forecast?latitude=33.6405&longitude=-117.8443&temperature_unit=fahrenheit&current_weather=true&daily=sunrise,sunset&timezone=America%2FLos_Angeles";
const char apiHostname[] = "api.open-meteo.com";
const char apiPath[] = "/v1/forecast?latitude=33.6405&longitude=-117.8443&temperature_unit=fahrenheit&current_weather=true&daily=sunrise,sunset&timezone=America%2FLos_Angeles";

// time server information
const char* NTP_SERVER = "pool.ntp.org";
const long GMT_OFFSET_SEC = -28800;  // offset btwn timezone and gmt -8
const int DAYLIGHT_OFFSET_SEC = 3600;

const int kNetworkTimeout = 30*1000;    // Number of milliseconds to wait without receiving any data before we give up
const int kNetworkDelay = 1000;         // Number of milliseconds to wait if no data is available before trying again

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

std::string httpRequest(const char* url){
    std::string payload = "";
    HTTPClient http;
    http.begin(url); //Specify the URL
    int httpCode = http.GET();              
    if (httpCode > 0) {                     
        payload = http.getString().c_str();
      } 
    else {
      Serial.println("Error on HTTP request");
    }
    http.end();
    return payload;
}

void httpPost(const char* serverName, const char* body){
    WiFiClient client;
    HTTPClient http;
    http.begin(client, serverName);

    // Specify content-type header
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    String httpRequestData = body;
    int httpResponseCode = http.POST(httpRequestData);
        
    // Free resources
    http.end();
}

StaticJsonDocument<1536> requestWeatherJson() {
    std::string resp =  httpRequest(API_URL);
    return jsonify(resp);
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

tm* getCurTime () {
    struct tm * t = new tm;
    configTime( GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_SERVER);
    if (!getLocalTime(t)){
         Serial.println("failed to obtain time");
    }
    return t;
}

void postTempAndLight(float outdoorTemp, float indoorTemp, float indoorLight, std::string time){
    char body[150];
    // how to pass in the time??
    sprintf(body, "time=%s&outdoorTemp=%.2f&indoorTemp=%.2f&indoorLight=%.2f", time.c_str(), outdoorTemp, indoorTemp, indoorLight);
    Serial.println(body);
    httpPost(AWS_TEMPLIGHT_ROUTE, body);
}

void postExceededSitting(unsigned int numExceededSitting){
    char body[150];
    sprintf(body, "numExceededSitting=%d", numExceededSitting);
    httpPost(AWS_EXCEEDSITTING_ROUTE, body);
}

void postSittingDuration(float sitDuration, std::string time){
    char body[150];
    sprintf(body, "time=%s&sittingDuration=%.2f", time.c_str(), sitDuration);
    httpPost(AWS_SITDUR_ROUTE, body);
}

void postNumDarkIndoors(unsigned int numDarkIndoors){
    char body[150];
    sprintf(body, "numDarkIndoors=%d", numDarkIndoors);
    httpPost(AWS_DARKINDOORS_ROUTE, body);
}

