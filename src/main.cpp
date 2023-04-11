#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>


// BME280 variables
#define SEALEVELPRESSURE_HPA (1026.4) // sea level pressure at ann arbor
Adafruit_BME280 bme; // I2C

// OLED display variables
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// LED variables
#define LED_PIN_R 2
#define LED_PIN_G 15


// helps declaration
void printValues();
void showBMEReadings(float temp, float humidity);
void LEDIndicator(float temp, float humidity);

//////////////////////////////////////////////////
/////     Main code
//////////////////////////////////////////////////

void setup() {
    Serial.begin(115200);
    bool bme_status;
    // default settings
    bme_status = bme.begin(0x76);  
    if (!bme_status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
    }

    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
    }
    delay(2000);
    display.clearDisplay();
    display.setTextColor(WHITE);

    pinMode(LED_PIN_R, OUTPUT);
    pinMode(LED_PIN_G, OUTPUT);

}

void loop() { 
    // printValues();
    delay(2000);
    float temp = bme.readTemperature();
    float humidity = bme.readHumidity();
    showBMEReadings(temp, humidity);
    LEDIndicator(temp, humidity);
}

//////////////////////////////////////////////////
/////     HELPERS
//////////////////////////////////////////////////
void showBMEReadings(float temp, float humidity){
  // clear display
    display.clearDisplay();
    // display the title
    display.setCursor(0, 0); //cursor(x, y)
    display.setTextSize(1.9);
    display.println("Monitoring...");

    // display temperature
    display.setTextSize(1);
    display.setCursor(0, 17);
    display.print("Temp: ");
    display.setTextSize(2);
    display.setCursor(35 ,17);
    display.print(temp);
    display.print(" C");
    
    // display humidity
    display.setTextSize(1);
    display.setCursor(0, 40);
    display.print("Humi");
    display.setCursor(0, 50);
    display.print("dity:");
    display.setTextSize(2);
    display.setCursor(35, 45);
    display.print(humidity);
    display.print(" %"); 
    
    display.display(); 
}

void LEDIndicator(float temp, float humidity){
    if (temp > 30 || humidity < 30){
        digitalWrite(LED_PIN_R, HIGH); // on
        delay(500);            
        digitalWrite(LED_PIN_R, LOW);  // off
        digitalWrite(LED_PIN_G, LOW);
        delay(500);     
    } else {
        digitalWrite(LED_PIN_G, HIGH);
        delay(500);  
    }
}

void printValues() {
    Serial.print("Temperature = ");
    Serial.print(bme.readTemperature());
    Serial.println(" *C");

    // Convert temperature to Fahrenheit
    /*Serial.print("Temperature = ");
    Serial.print(1.8 * bme.readTemperature() + 32);
    Serial.println(" *F");*/

    Serial.print("Pressure = ");
    Serial.print(bme.readPressure() / 100.0F);
    Serial.println(" hPa");

    Serial.print("Approx. Altitude = ");
    Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
    Serial.println(" m");

    Serial.print("Humidity = ");
    Serial.print(bme.readHumidity());
    Serial.println(" %");

    Serial.println();
}