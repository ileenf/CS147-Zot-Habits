 #include "helper.hpp"

 float convToSec(float ms){
   return ms / 1000;
 }

 void calibrateLightSensor(int& minLightVal, int& maxLightVal, int photoResPin){

    int start_time = millis();
    int time_elapsed = millis() - start_time;

    // ten seconds
    Serial.println("Starting light sensor calibration for 10 secs...");
    sleep(2);
    
    while (time_elapsed < 10000){
      int cur_light_val =  analogRead(photoResPin);
      Serial.println(cur_light_val);

      minLightVal = min(cur_light_val, minLightVal);
      maxLightVal = max(cur_light_val, maxLightVal);
      time_elapsed = millis() - start_time;
  }
  Serial.print("Min = "); Serial.println(minLightVal);
  Serial.print("Max = "); Serial.println(maxLightVal);
 }

 float getLightVal(float minLight, float maxLight, float rawVal){
   // val between 0 and 100
   return (100 * (rawVal - minLight)) / (maxLight - minLight);
 }
 
 void printWeatherData(float outsideTemp, tm* sunriseTm, tm* sunsetTm){
      Serial.println("-------- Weather data from API ---------");
      Serial.print("Outside temp = "); Serial.println(outsideTemp);
      Serial.print("Sunrise = "); Serial.println(sunriseTm, "%A, %B %d %Y %I:%M:%S");
      Serial.print("Sunset = "); Serial.println(sunsetTm, "%A, %B %d %Y %I:%M:%S");
    }  

void printSensorData(float temp, float light){
  printIndoorTemp(temp);
  printIndoorLight(light);
}

void printCurTime(tm* curTm){
      Serial.println(curTm, "Current time =   %A, %B %d %Y %I:%M:%S");
    }

void printIndoorTemp(float temp){
      Serial.print("Indoor temperature =   ");
      Serial.println(temp);
    }

void printOutdoorTemp(float temp){
      Serial.print("Outdoor temperature =   ");
      Serial.println(temp);
    }

void printIndoorLight(int lightVal){
    Serial.print("Indoor light =    ");
    Serial.println(lightVal);
}

void printTempRec(int rec){
    Serial.print("Temperature Recommendation =  ");
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

void printLightRec(int rec){
    Serial.println("Light Recommendation:   ");
      if (rec == DO_NOTHING){
        Serial.println("Do nothing");
      }
      else if (rec == OPEN_BLINDS){
        Serial.println("Open blinds");
      }
      else if (rec == TURN_ON_LIGHT){
        Serial.println("Turn on light");
      }
      Serial.println();
    }

int giveTempRec(float indoor, float outdoor, float prefTemp){
      if (outdoor > indoor && indoor < prefTemp){
          // OPEN WINDOW TO WARM UP HOUSE b/c it's too cold inside
          return OPEN_WINDOW;
      }
      if (outdoor > indoor && indoor >= prefTemp){
          // CLOSE WINDOW, TURN ON AC b/c too hot outside
          return CLOSE_WINDOW;
      }
      if (outdoor < indoor && indoor > prefTemp){
          // OPEN WINDOW TO COOL DOWN HOUSE b/c it's too warm inside
          return OPEN_WINDOW;
      }
      if (outdoor < indoor && indoor <= prefTemp){
          // CLOSE WINDOW, TURN ON HEATER b/c it's too cold outside
          return CLOSE_WINDOW;
      }
      // if outdoor == indoor
      return DO_NOTHING;
}   

int tmCompare(tm* t1, tm* t2){
  if (t1->tm_hour > t2->tm_hour){       // t1 hour > t2 hour
      return 1;
  }
  else if (t1->tm_hour < t2->tm_hour){  // t1 hour < t2 hour
      return -1;
  }
  else {                                // hours equal
      if (t1->tm_min > t2->tm_min){
        return 1;
      }
      else if (t1->tm_min < t2->tm_min){
        return -1;
      }
      else {                               // mins equal
          if (t1->tm_sec > t2->tm_sec){
            return 1;
          }
          else if (t1->tm_sec < t2->tm_sec){
            return -1;
          }
      }
  }
  // secs equal
  return 0;
}

bool isDarkIndoors(float curLightVal){
    return curLightVal < LIGHT_THRESHOLD;
}

bool isDaytime(tm* curTm, tm* sunset, tm* sunrise){
  return (tmCompare(curTm, sunrise) >= 0 && tmCompare(curTm, sunset) < 1);
}

int giveLightRec(tm* curTm, tm* sunset, tm* sunrise, float curLightVal){
    if (isDarkIndoors(curLightVal)){
        // If it’s daytime outside, open blinds -- curTm >= sunrise && curTm <= sunset
        if (isDaytime(curTm, sunset, sunrise)){
            return OPEN_BLINDS;
        } 
        else {
          // If it's nighttime, turn on lights
          return TURN_ON_LIGHT;
        }
    }
    // else, light levels are sufficient
    return DO_NOTHING;
}

void getTimeString(tm* curTm, std::string& str){
  char timeStr[100];
  sprintf(timeStr, "%d:%d:%d", curTm->tm_hour, curTm->tm_min, curTm->tm_sec);
  str = timeStr;
}



    
    