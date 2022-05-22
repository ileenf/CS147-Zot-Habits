#include <Arduino.h>
#include <Wire.h>
#include "DHT20.h"
#include "TempSensor.hpp"
#include "wifiConnect.hpp"

const int trigPin = GPIO_NUM_25;
const int echoPin = GPIO_NUM_15;
const int motionSensor = GPIO_NUM_13;
const int photoResistorPin = GPIO_NUM_32;

#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701

const int DO_NOTHING = 0;
const int OPEN_WINDOW = 1;
const int CLOSE_WINDOW = 2;


long duration;
float distanceCm;
float distanceInch;

StaticJsonDocument<BUF_SIZE> weather_json;

namespace {
    void print_weather_data(float outside_temp, tm* sunrise_tm, tm* sunset_tm){
      Serial.println("-------- Weather data from API ---------");
      Serial.print("Outside temp = "); Serial.println(outside_temp);
      Serial.print("Sunrise = "); Serial.println(sunrise_tm, "%A, %B %d %Y %I:%M:%S");
      Serial.print("Sunset = "); Serial.println(sunset_tm, "%A, %B %d %Y %I:%M:%S");
    }  
    void print_cur_time(tm* cur_tm){
      Serial.println("-------- Current time ---------");
      Serial.println(cur_tm, "%A, %B %d %Y %I:%M:%S");
    }
    void print_indoor_temp(float temp){
      Serial.println("-------- Indoor temperature (F) ---------");
      Serial.println(temp);
    }
    void print_outdoor_temp(float temp){
      Serial.println("-------- Outdoor temperature (F) ---------");
      Serial.println(temp);
    }
    void print_rec(int rec){
      if (rec == DO_NOTHING){
        Serial.println("Do nothing");
      }
      else if (rec == OPEN_WINDOW){
        Serial.println("Open window");
      }
      else if (rec == CLOSE_WINDOW){
        Serial.println("Close window");
      }
    }

    int giveRec(float indoor, float outdoor, float pref_temp){

      if (outdoor > indoor && indoor < pref_temp){
          // OPEN WINDOW TO WARM UP HOUSE b/c it's too cold inside
          return OPEN_WINDOW;
      }
      if (outdoor > indoor && indoor >= pref_temp){
          // CLOSE WINDOW, TURN ON AC b/c too hot outside
          return CLOSE_WINDOW;
      }
      if (outdoor < indoor && indoor > pref_temp){
          // OPEN WINDOW TO COOL DOWN HOUSE b/c it's too warm inside
          return OPEN_WINDOW;
      }
      if (outdoor < indoor && indoor <= pref_temp){
          // CLOSE WINDOW, TURN ON HEATER b/c it's too cold outside
          return CLOSE_WINDOW;
      }

      // if outdoor == indoor
      return DO_NOTHING;
      
    }   
    
    
}

// initialize temp/humidity sensor
TempSensor tempSensor{75}; 

void IRAM_ATTR detectsMovement(){
  // Serial.println("MOTION DETECTED!!!");
}

void setup() {
  Serial.begin(115200);

  // distance sensor
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // motion sensor
  pinMode(motionSensor, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(motionSensor), detectsMovement, RISING);

  // Connect to WiFi network
  delay(1000);
  Serial.println();
  Serial.println();

  initializeWifi();

  // Weather API request
  weather_json = getWeatherJson();
  
  float outside_temp = get_outside_temp(weather_json);
  tm* cur_tm = get_cur_time();
  tm* sunset_tm = get_daily_sunset(weather_json);
  tm* sunrise_tm = get_daily_sunrise(weather_json);

  print_cur_time(cur_tm);
  print_weather_data(outside_temp, sunrise_tm, sunset_tm);

  delete sunset_tm;
  delete sunrise_tm;
  delete cur_tm;

    
}

void loop() {
  //distance sensor -- emit signal 
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // distance sensor -- measure time it takes for signal to return 
  duration = pulseIn(echoPin, HIGH);
  distanceCm = duration * SOUND_SPEED/2;
  distanceInch = distanceCm * CM_TO_INCH;
  Serial.println(distanceInch);

  // light sensor 
  // int currLightVal = analogRead(photoResistorPin);
  // Serial.println(currLightVal);

  //temp sensor
  // weather_json = getWeatherJson();
  // float outdoor_temp = get_outside_temp(weather_json);
  // float indoor_temp= tempSensor.readIndoorTemp();
  // float pref_temp = tempSensor.getPreferredTemp();

  // print_indoor_temp(indoor_temp);
  // print_outdoor_temp(outdoor_temp);
  
  // int rec = giveRec(indoor_temp, outdoor_temp, pref_temp);
  // print_rec(rec);

  delay(1000);
  
}