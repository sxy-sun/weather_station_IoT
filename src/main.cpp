#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>


/////     BME280 variables
#define SEALEVELPRESSURE_HPA (1026.4) // sea level pressure at ann arbor
Adafruit_BME280 bme; // I2C
unsigned long delayTime;

/////     OLED display variables
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

/////     button variabled
#define BUTTON_PIN 36  // GIOP36 pin connected to button
// Variables will change:
int lastState = LOW;  // the previous state from the input pin
int currentState;     // the current reading from the input pin

/////     helps declaration
void printValues();
void showBMEReadings(float temp, float humidity);
void button();

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

  pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void loop() { 
  // printValues();
  delay(2000);
  float temp = bme.readTemperature();
  float humidity = bme.readHumidity();
  button();
  showBMEReadings(temp, humidity);
  delay(delayTime);
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

void button(){
  // read the state of the switch/button:
  currentState = digitalRead(BUTTON_PIN);

  if (lastState == HIGH && currentState == LOW) {
    Serial.println("The button is pressed");
  }
  else if (lastState == LOW && currentState == HIGH){
    Serial.println("The button is released");
  }
  lastState = currentState;
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