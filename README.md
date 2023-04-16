# IoT weather station with ESP32

This IoT project can retrieve local weather from the internet 
while detecting room temperature and humidity and display two sets of readings on an OLED display. More information about implementation can be found [here]().

## How to use
This project is precisely feasible with Umich on-campus wifi, and users can seamlessly
connect to MWireless by uncommenting these two lines and enter the info accordingly.
```c++
#define EAP_IDENTITY "username@umich.edu" //enter full umich email address
#define EAP_PASSWORD "umich_password"     //your umich password
```

To use the OpenWeather API, the user needs to uncomment this line and enter
one's own token.
```c++
String openWeatherAPI = "http://api.openweathermap.org/data/2.5/weather?zip=48105,us&APPID=yourOpenWeatherToken";
```

## Hardware
- Motherboard: ESP32
- Environmental sensor: BME280
- Display: OLED display [(Amazon)](https://www.amazon.com/dp/B0B7RPCZ4Z?psc=1&ref=ppx_yo2ov_dt_b_product_details)

## Software
- IDE: vscode + PlatformIO extension

## Video demo
[![IMAGE ALT TEXT](http://img.youtube.com/vi/YoT-wcManlM/0.jpg)](http://www.youtube.com/watch?v=YoT-wcManlM "IoT Weather Station with ESP32 Demo")

