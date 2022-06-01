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

unsigned int numDarkIndoors = 0;
bool isBreak = false;
bool stoodUp = false;
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
int minLightVal = INT_MAX;
int maxLightVal = INT_MIN;

StaticJsonDocument<BUF_SIZE> weatherJson;

TempSensor tempSensor{75, true};           // prefTemp = 75, units in Fahrenheit
Distance distanceSensor{trigPin, echoPin};

void IRAM_ATTR detectsMovement(){
  movementDetected = true;
  Serial.println("MOTION DETECTED");
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

  // light sensor
  calibrateLightSensor(minLightVal, maxLightVal, photoResistorPin);

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

  tm* curTm = getCurTime(); 
  std::string timeStr = "";
  getTimeString(curTm, timeStr);

  // distance sensor -- measure time it takes for signal to return 
  float distance = getCurrentDistance();
  currentSittingTime = millis() - initialSittingTime;
  
  Serial.print("distance to closest object: "); Serial.println(distance);
  Serial.print("threshold: "); Serial.println(distanceSensor.getDistanceThreshold());
  Serial.print("current sitting time (sec): "); Serial.println(convToSec(currentSittingTime));
  Serial.println();

  if (currentSittingTime > maxSittingTime && !isBreak) {      // user needs to take a break now, buzzer to start break
    Serial.println(" BREAK TIME!!! ");
    playBuzzer();
    if (distanceSensor.distanceGreaterThanThreshold() && movementDetected) {
      // person has moved away
      Serial.println("-- BREAK TIME, person has moved away --");
      stoodUp = true;
      initialSittingTime = millis();
      movementDetected = false;

      Serial.print("sittingDuration: "); Serial.println(currentSittingTime);
      postSittingDuration(currentSittingTime, timeStr); //how long they sat for

    } else {  // send to aws to notify user has exceeded their sitting time
      Serial.println(" BREAK TIME, person stayed sitting ");
      exceededSittingTime++;
      stoodUp = false;
      Serial.print("exceededSittingTime: "); Serial.println(exceededSittingTime);
      postExceededSitting(exceededSittingTime); // num times that user exceeded sitting time, sends every second during break!!!
    }
    isBreak = true;
    startBreak = millis();

  }

  if (distanceSensor.distanceGreaterThanThreshold() && movementDetected) {
    // person has moved away
    Serial.println("-- NOT BREAK, person has moved away --");
    initialSittingTime = millis();
    movementDetected = false;
  }

  // buzzer when break is over
  if (isBreak && millis() - startBreak >= breakDuration) {
    Serial.println(" BREAK OVER");
    playBuzzer();
    isBreak = false;
    if (stoodUp){
      initialSittingTime = millis();
    }
  }
  
    // temp sensor
    weatherJson = requestWeatherJson();
    
    tm* sunsetTm = getDailySunset(weatherJson);
    tm* sunriseTm = getDailySunrise(weatherJson);
    float outdoorTemp = getOutsideTemp(weatherJson);
    float indoorTemp = tempSensor.readIndoorTemp();
    float prefTemp = tempSensor.getPreferredTemp();
    int rec = giveTempRec(indoorTemp, outdoorTemp, prefTemp);

    //light sensor
    int currLightVal = getLightVal(minLightVal, maxLightVal, analogRead(photoResistorPin));
    int lightRec = giveLightRec(curTm, sunsetTm, sunriseTm, currLightVal);
    
    

    printCurTime(curTm);
    printWeatherData(outdoorTemp, sunriseTm, sunsetTm);
    printIndoorTemp(indoorTemp);
    printOutdoorTemp(outdoorTemp);
    printIndoorLight(currLightVal);

    printTempRec(rec);
    printLightRec(lightRec);

    if (lightRec == OPEN_BLINDS || lightRec == TURN_ON_LIGHT){
      numDarkIndoors++;
      postNumDarkIndoors(numDarkIndoors);// number of times it was dark indoors
    }
    postTempAndLight(outdoorTemp, indoorTemp, currLightVal, timeStr);

    delete sunsetTm;
    delete sunriseTm;
    delete curTm;

   

    delay(3000);
}

