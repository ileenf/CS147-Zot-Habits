#include <Arduino.h>
#include <Wire.h>
#include "DHT20.h"
#include "TempSensor.hpp"
#include "wifiConnect.hpp"
#include "Motion.hpp"
#include "Distance.hpp"
#include <Tone32.h>
#include "helper.hpp"

const int trigPin = GPIO_NUM_25;  
const int echoPin = GPIO_NUM_15;
const int motionSensorPin = GPIO_NUM_13;
const int photoResistorPin = GPIO_NUM_32; // 25 in use by wifi
const int buzzerPin = GPIO_NUM_13;

#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701
#define BUZZER_CHANNEL 0

bool isBreak = false;
float maxSittingTime = 20000;
float initialSittingTime;
float currentSittingTime;
float startBreak;
float breakDuration = 10000;
long duration;
float distanceCm;
float distanceInch;
float volatile movementDetected = false;
int exceededSittingTime = 0;
char sittingStr[] = "numExceededSitting";
char outdoorStr[] = "outdoorTemp";
char indoorStr[] = "indoorTemp";

StaticJsonDocument<BUF_SIZE> weatherJson;

TempSensor tempSensor{75, true};           // prefTemp = 75, units in Fahrenheit
Distance distanceSensor{trigPin, echoPin};

void IRAM_ATTR detectsMovement(){
  movementDetected = true;
}

float getCurrentDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distanceSensor.setDistanceInch(duration);
  float distanceInch = distanceSensor.getDistanceInch();
  return distanceInch;
}

void playBuzzer() {
  float startTime = millis();
  while (millis() - startTime < 1500) {
    tone(buzzerPin, NOTE_C4, 500, BUZZER_CHANNEL);
    noTone(buzzerPin, BUZZER_CHANNEL);
  }
}

void setup() {
  Serial.begin(115200);

  // distance sensor
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // buzzer
  pinMode(buzzerPin, OUTPUT);

  // initialize sitting time
  initialSittingTime = millis();

  // intialize distance threshold (distance from sitting down position)
  float threshold = getCurrentDistance();
  distanceSensor.initializeDistanceThreshold(threshold);

  // motion sensor
  pinMode(motionSensorPin, INPUT);
  attachInterrupt(motionSensorPin, detectsMovement, RISING);

  // Connect to WiFi network
  delay(1000);
  Serial.println();
  Serial.println();
  initializeWifi();
}

void loop() {
  // distance sensor -- measure time it takes for signal to return 
  float distance = getCurrentDistance();
  Serial.print("distance: ");
  Serial.println(distance);
  Serial.print("threshold: ");
  Serial.println(distanceSensor.getDistanceThreshold());
  currentSittingTime = millis() - initialSittingTime;
  Serial.print("current sitting time: ");
  Serial.println(currentSittingTime);

  if (currentSittingTime > maxSittingTime && !isBreak) {
    // user needs to take a break now, buzzer to start break
    if (distanceSensor.distanceGreaterThanThreshold() && movementDetected) {
      // person has moved away
      Serial.println("person has moved away");
      initialSittingTime = millis();
      movementDetected = false;
      playBuzzer();
      isBreak = true;
      startBreak = millis();

      // send to aws all times without standing up
      char sittingDuration[] = "sittingDuration";
      sendData(sittingDuration, currentSittingTime);

  } else {
      // send to aws to notify user has exceeded their sitting time
      exceededSittingTime++;
      sendData(sittingStr, exceededSittingTime);
    }
  }
  Serial.print("movement detected: ");
  Serial.println(movementDetected);
  if (distanceSensor.distanceGreaterThanThreshold() && movementDetected) {
    // person has moved away
    Serial.println("person has moved away");
    initialSittingTime = millis();
    movementDetected = false;
  }

  // buzzer when break is over
  if (isBreak && millis() - startBreak >= breakDuration) {
    playBuzzer();
    isBreak = false;
    initialSittingTime = millis();
  }
  
  //temp sensor
  weatherJson = requestWeatherJson();
  tm* curTm = getCurTime();
  tm* sunsetTm = getDailySunset(weatherJson);
  tm* sunriseTm = getDailySunrise(weatherJson);
  float outdoorTemp = getOutsideTemp(weatherJson);
  printWeatherData(outdoorTemp, sunriseTm, sunsetTm);

  float indoorTemp = tempSensor.readIndoorTemp();
  float prefTemp = tempSensor.getPreferredTemp();
  int rec = giveTempRec(indoorTemp, outdoorTemp, prefTemp);

  printIndoorTemp(indoorTemp);
  printOutdoorTemp(outdoorTemp);
  printTempRec(rec);

  // send outdoor temp data to aws
  sendData(outdoorStr, outdoorTemp);
  Serial.println(outdoorTemp);
  delay(2000);

  // send indoor temp data to aws
  sendData(indoorStr, indoorTemp);
  Serial.println(indoorTemp);

  // light sensor 
  int currLightVal = analogRead(photoResistorPin);
  int lightRec = giveLightRec(curTm, sunsetTm, sunriseTm, currLightVal);
  
  printCurTime(curTm);
  printIndoorLight(currLightVal);
  printLightRec(lightRec);

  delete sunsetTm;
  delete sunriseTm;
  delete curTm;
}

