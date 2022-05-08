#include <Arduino.h>
#include <Wire.h>
#include "DHT20.h"
#include "wifiConnect.hpp"

const int trigPin = GPIO_NUM_25;
const int echoPin = GPIO_NUM_15;
const int motionSensor = GPIO_NUM_13;
const int photoResistorPin = GPIO_NUM_32;

#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701

long duration;
float distanceCm;
float distanceInch;

// initialize temp/humidity sensor
DHT20 DHT;

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
  
  // temp/humidity sensor
  DHT.begin();

  // Connect to WiFi network
  delay(1000);
  Serial.println();
  Serial.println();

  initializeWifi();
  getWeatherData();
}

void loop() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distanceCm = duration * SOUND_SPEED/2;
  distanceInch = distanceCm * CM_TO_INCH;

  int currLightVal = analogRead(photoResistorPin);
  Serial.print(currLightVal);

  // HTTP Requests
  delay(1000);
  httpRequest("worldtimeapi.org", "/api/timezone/Europe/London.txt");
  
}