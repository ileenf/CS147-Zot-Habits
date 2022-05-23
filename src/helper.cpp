 #include "helper.hpp"
 
 void printWeatherData(float outside_temp, tm* sunrise_tm, tm* sunset_tm){
      Serial.println("-------- Weather data from API ---------");
      Serial.print("Outside temp = "); Serial.println(outside_temp);
      Serial.print("Sunrise = "); Serial.println(sunrise_tm, "%A, %B %d %Y %I:%M:%S");
      Serial.print("Sunset = "); Serial.println(sunset_tm, "%A, %B %d %Y %I:%M:%S");
    }  

void printCurTime(tm* cur_tm){
      Serial.println("-------- Current time ---------");
      Serial.println(cur_tm, "%A, %B %d %Y %I:%M:%S");
    }

void printIndoorTemp(float temp){
      Serial.println("-------- Indoor temperature ---------");
      Serial.println(temp);
    }

void printOutdoorTemp(float temp){
      Serial.println("-------- Outdoor temperature ---------");
      Serial.println(temp);
    }

void printRec(int rec){
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
    
    