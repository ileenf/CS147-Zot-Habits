# CS147 Project - Zot Habits

Zot Habits is a IoT software project developed by Ileen Fan and Sona Shridhar which utilizes an ESP32 development board, various sensors, connection to an AWS cloud web server, and a web interface to display aggregated data and habit recommendations.

## Overview
Our motivation for this project is to support college students to have good healthy work habits, live sustainably, and be environmentally aware. Our goals are to leverage outdoor conditions and real time sensing to lessen the use of artificial light, temperature, and encourage students to have healthier lifestyle habits, such as not sitting too long and not working in the dark . We want to help students be accountable for their actions and promote a better lifestyle and wellbeing.  Our solution consists of an arrangement of sensors that collects environmental data and compares it to outdoor data (outdoor temperature, sunset and sunrise times obtained from an API) to draw conclusions about the user’s work environment. These conclusions as well as raw sensor data are sent via WiFi to an AWS Flask server. For storing the data, we saved the raw sensor values to local files in our web server. To display the data, we read and parsed through the files. We had different files to store light/temp data, sitting data, health data, etc. 

### Sensors Used
- ultrasonic distance sensor
- motion sensor
- photoresistor
- Humidity and Temperature Sensor

### Hardware Components
- ESP 32
- Breadboard
- Buzzer
- Wires/Resistors

### Data Collection
#### From Sensors
We retrieved and stored the different data and statistics we sensed from the user. The data aggregated include indoor/outdoor temperature, indoor light levels, sitting durations, and health trends. We also calculated how many times the user didn’t get up for their break, and how many times the user was working in a dark room, which are both components in the overall health percentage calculation.

#### From Weather API
We used the [Open Meteo](https://open-meteo.com/en) Weather API to get the outdoor temperature and sunset/sunrise times. We compared the sunset/sunrise times to the user's current time to determine is it's day time or night time outside. We then compared the API data to the sensor data to make conclusions about user behavior.

API Query Parameters:

    latitude
    longitude
    temperature_unit
    current_weather
    daily
    timezone
    

### AWS Cloud Web Server
The ESP32 is connected to the temperature sensor, motion sensor, distance sensor, buzzer, and photoresistor. The temperature sensor and photoresistor collect data every minute, while the motion and distance sensor sense movement more frequently, every 3 seconds. The buzzer sounds to signal the beginning and end of breaks. The ESP32 uses WiFi communication protocol in order to send data to the AWS Flask server, which stores the sensor data locally in text files and displays graphs on a web interface. We made POST requests to send sensor collected data (light, temperature, sitting statistics, etc) to the AWS server. 

### Web Interface
The web interface includes features such as displaying graphs, data aggregation, statistics about user habits, and lifestyle recommendations with real time updates from AWS. We used Flask and HTML to render the interface. We used matplotlib to plot graphs for sitting durations, indoor vs outdoor temp, light values, and overall healthiness over time. 

## System Architecture
<img width="665" alt="System Architecture Picture" src="https://user-images.githubusercontent.com/52259696/171965273-6caa4e1d-49a3-44c5-b1ba-c6300258f849.png">

## Screenshots
<img width="911" alt="Graphs" src="https://user-images.githubusercontent.com/52259696/171965403-c2a66b07-819d-45b8-9db1-b453a0be3e91.png">
The sensor data is plotted on graphs and the corresponding aggregations (mean, min, max). At the top we show the overall healthiness percentage.


<img width="985" alt="Statistics" src="https://user-images.githubusercontent.com/52259696/171965512-0f452990-8dd1-4c8f-b013-ec66b3a73c00.png">
Statistics about user behaviors are displayed along with recommendations for healthier habits/lifestyle.

## Contributors
[@ileenf](https://github.com/ileenf)

[@shridhar](https://github.com/sonashridhar)
