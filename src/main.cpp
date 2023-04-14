#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <esp_wpa2.h>
#include <esp_wifi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>

// SECRET


// open weather variables
// String openWeatherAPI = "http://api.openweathermap.org/data/2.5/weather?zip=48105,us&APPID=yourOpenWeatherToken";
double local_temp = -100;
double local_temp_min = -100;
double local_temp_max = -100;
unsigned long lastTimeHTTP = 0;
unsigned long timerDelayHTTP = 20000;
// unsigned long timerDelayHTTP = 10000;
String jsonBuffer;

// IFTTT variables 
const char* IFTTT_host = "maker.ifttt.com";
// const char* IFTTT_apikey = "Your IFTTT api key";
unsigned long lastTimeIFTTT = 0;
// unsigned long timerDelayIFTTT = 3600000;
unsigned long timerDelayIFTTT = 30000;


// BME280 variables
#define SEALEVELPRESSURE_HPA (1026.4) // sea level pressure at ann arbor
Adafruit_BME280 bme; // I2C

// OLED display variables
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Button variable
#define BUTTON_PIN 4
bool showBME;
bool showWeather;

// LED variables
#define LED_PIN_R 2
#define LED_PIN_G 12

// wifi setup
const char *ssid = "eduroam"; // Eduroam // MWireless seized all 2.4GHz SSID on 2/25/2020
// #define EAP_IDENTITY "username@umich.edu" //enter full umich email address
// #define EAP_PASSWORD "umich_password"     //your umich password


static const char incommon_ca[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIF+TCCA+GgAwIBAgIQRyDQ+oVGGn4XoWQCkYRjdDANBgkqhkiG9w0BAQwFADCB
iDELMAkGA1UEBhMCVVMxEzARBgNVBAgTCk5ldyBKZXJzZXkxFDASBgNVBAcTC0pl
cnNleSBDaXR5MR4wHAYDVQQKExVUaGUgVVNFUlRSVVNUIE5ldHdvcmsxLjAsBgNV
BAMTJVVTRVJUcnVzdCBSU0EgQ2VydGlmaWNhdGlvbiBBdXRob3JpdHkwHhcNMTQx
MDA2MDAwMDAwWhcNMjQxMDA1MjM1OTU5WjB2MQswCQYDVQQGEwJVUzELMAkGA1UE
CBMCTUkxEjAQBgNVBAcTCUFubiBBcmJvcjESMBAGA1UEChMJSW50ZXJuZXQyMREw
DwYDVQQLEwhJbkNvbW1vbjEfMB0GA1UEAxMWSW5Db21tb24gUlNBIFNlcnZlciBD
QTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAJwb8bsvf2MYFVFRVA+e
xU5NEFj6MJsXKZDmMwysE1N8VJG06thum4ltuzM+j9INpun5uukNDBqeso7JcC7v
HgV9lestjaKpTbOc5/MZNrun8XzmCB5hJ0R6lvSoNNviQsil2zfVtefkQnI/tBPP
iwckRR6MkYNGuQmm/BijBgLsNI0yZpUn6uGX6Ns1oytW61fo8BBZ321wDGZq0GTl
qKOYMa0dYtX6kuOaQ80tNfvZnjNbRX3EhigsZhLI2w8ZMA0/6fDqSl5AB8f2IHpT
eIFken5FahZv9JNYyWL7KSd9oX8hzudPR9aKVuDjZvjs3YncJowZaDuNi+L7RyML
fzcCAwEAAaOCAW4wggFqMB8GA1UdIwQYMBaAFFN5v1qqK0rPVIDh2JvAnfKyA2bL
MB0GA1UdDgQWBBQeBaN3j2yW4luHS6a0hqxxAAznODAOBgNVHQ8BAf8EBAMCAYYw
EgYDVR0TAQH/BAgwBgEB/wIBADAdBgNVHSUEFjAUBggrBgEFBQcDAQYIKwYBBQUH
AwIwGwYDVR0gBBQwEjAGBgRVHSAAMAgGBmeBDAECAjBQBgNVHR8ESTBHMEWgQ6BB
hj9odHRwOi8vY3JsLnVzZXJ0cnVzdC5jb20vVVNFUlRydXN0UlNBQ2VydGlmaWNh
dGlvbkF1dGhvcml0eS5jcmwwdgYIKwYBBQUHAQEEajBoMD8GCCsGAQUFBzAChjNo
dHRwOi8vY3J0LnVzZXJ0cnVzdC5jb20vVVNFUlRydXN0UlNBQWRkVHJ1c3RDQS5j
cnQwJQYIKwYBBQUHMAGGGWh0dHA6Ly9vY3NwLnVzZXJ0cnVzdC5jb20wDQYJKoZI
hvcNAQEMBQADggIBAC0RBjjW29dYaK+qOGcXjeIT16MUJNkGE+vrkS/fT2ctyNMU
11ZlUp5uH5gIjppIG8GLWZqjV5vbhvhZQPwZsHURKsISNrqOcooGTie3jVgU0W+0
+Wj8mN2knCVANt69F2YrA394gbGAdJ5fOrQmL2pIhDY0jqco74fzYefbZ/VS29fR
5jBxu4uj1P+5ZImem4Gbj1e4ZEzVBhmO55GFfBjRidj26h1oFBHZ7heDH1Bjzw72
hipu47Gkyfr2NEx3KoCGMLCj3Btx7ASn5Ji8FoU+hCazwOU1VX55mKPU1I2250Lo
RCASN18JyfsD5PVldJbtyrmz9gn/TKbRXTr80U2q5JhyvjhLf4lOJo/UzL5WCXED
Smyj4jWG3R7Z8TED9xNNCxGBMXnMete+3PvzdhssvbORDwBZByogQ9xL2LUZFI/i
eoQp0UM/L8zfP527vWjEzuDN5xwxMnhi+vCToh7J159o5ah29mP+aJnvujbXEnGa
nrNxHzu+AGOePV8hwrGGG7hOIcPDQwkuYwzN/xT29iLp/cqf9ZhEtkGcQcIImH3b
oJ8ifsCnSbu0GB9L06Yqh7lcyvKDTEADslIaeSEINxhO2Y1fmcYFX/Fqrrp1WnhH
OjplXuXE0OPa0utaKC25Aplgom88L2Z8mEWcyfoB7zKOfD759AN7JKZWCYwk
-----END CERTIFICATE-----
)EOF";

// helps declaration
void printValues();
void showBMEReadings(float temp, float humidity, bool showBME);
void LEDIndicator(float temp, float humidity);
void openWeather();
String httpGETRequest(const char* serverName);
void IFTTT(float humidity);

//////////////////////////////////////////////////
/////     Main code
//////////////////////////////////////////////////

void setup() {
    Serial.begin(115200);

    // setup for bme
    bool bme_status;
    bme_status = bme.begin(0x76);  
    if (!bme_status) {
        Serial.println("Could not find a valid BME280 sensor, check wiring!");
        while (1);
    }
    // set up for button
    pinMode(BUTTON_PIN, INPUT_PULLUP);

    // setup for display
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
        Serial.println(F("SSD1306 allocation failed"));
        for(;;);
    }
    display.clearDisplay();
    display.setTextColor(WHITE);
    showBME = true;
    showWeather = false;

    // setup for LED
    pinMode(LED_PIN_R, OUTPUT);
    pinMode(LED_PIN_G, OUTPUT);

    // setup for wifi
    WiFi.disconnect(true); //disconnect form wifi to set new wifi connection
    WiFi.mode(WIFI_STA);   //init wifi mode
    Serial.println("Connecting");
    Serial.print("MAC >> ");
    Serial.println(WiFi.macAddress());
    Serial.printf("Connecting to WiFi: %s ", ssid);
    esp_wifi_sta_wpa2_ent_set_ca_cert((uint8_t *)incommon_ca, strlen(incommon_ca) + 1);
    esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY));
    esp_wifi_sta_wpa2_ent_set_username((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY));
    esp_wifi_sta_wpa2_ent_set_password((uint8_t *)EAP_PASSWORD, strlen(EAP_PASSWORD));
    esp_wifi_sta_wpa2_ent_enable();
    WiFi.begin(ssid);
    while (WiFi.status() != WL_CONNECTED) {
        digitalWrite(LED_PIN_R, HIGH);
        delay(500);     
        digitalWrite(LED_PIN_R, LOW);
        delay(500);    
        Serial.printf(".");
    }
    Serial.println("Connected to WiFi network with IP Address: ");
    Serial.print(WiFi.localIP());
    WiFi.setAutoReconnect(true);
    WiFi.persistent(true);
}

void loop() { 
    delay(500);
    int currentState = digitalRead(BUTTON_PIN);
    if (currentState == LOW && showBME == false) {
        Serial.println("pressed! BME your turn!");
        showBME = true;
        showWeather = false;
    } else if (currentState == LOW && showBME == true) {
        Serial.println("pressed! Weather your turn!");
        showBME = false;
        showWeather = true;
    }
    float temp = bme.readTemperature();
    float humidity = bme.readHumidity();
    showBMEReadings(temp, humidity, showBME);
    LEDIndicator(temp, humidity);
    openWeather();

    if (showWeather) {
        display.clearDisplay();
        // display the title
        display.setCursor(0, 0); //cursor(x, y)
        display.setTextSize(1.9);
        display.setCursor(0, 5);
        display.println("Temp in 3 hours");

        display.setTextSize(1);
        display.setCursor(0, 20);
        display.print("Cur Temp: ");
        display.print(local_temp);
        display.print(" C");

        display.setTextSize(1);
        display.setCursor(0, 35);
        display.print("Min Temp: ");
        display.print(local_temp_min);
        display.print(" C");
        
        display.setTextSize(1);
        display.setCursor(0, 50);
        display.print("Max Temp: ");
        display.print(local_temp_max);
        display.print(" C"); 
        
        display.display(); 
    }
}

//////////////////////////////////////////////////
/////     HELPERS
//////////////////////////////////////////////////
void showBMEReadings(float temp, float humidity, bool showBME){
  // clear display
    if (showBME){
        delay(500);
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
}

void LEDIndicator(float temp, float humidity){
    if (humidity < 30 || humidity > 50){
        digitalWrite(LED_PIN_R, HIGH); 
        digitalWrite(LED_PIN_G, LOW);
        IFTTT(humidity);
    } else {
        digitalWrite(LED_PIN_G, HIGH);
        digitalWrite(LED_PIN_R, LOW);   
    }
}

void IFTTT(float humidity) {
    if ((millis() - lastTimeIFTTT) > timerDelayIFTTT) {
        Serial.print("humidity: ");
        Serial.println(humidity);

        WiFiClient client;
        const int httpPort = 80;
        if (!client.connect(IFTTT_host, httpPort)) {
            Serial.println("connection failed");
            return;
        }       
        String slack_url = "/trigger/slack_alert/with/key/";
        slack_url += IFTTT_apikey;
        String jsonData;
        if (humidity < 30) {
            jsonData = String("{ \"value1\" : \"") + "The humidity is lower than 30 percent, " + "\", \"value2\" :\"" + "stay hydrated!" + "\", \"value3\" : \" \" }";
        } else {
            jsonData = String("{ \"value1\" : \"") + "The humidity is above than 50 percent, " + "\", \"value2\" :\"" + "food gets spoiled quickly!" + "\", \"value3\" : \" \" }";
        }
        client.print(String("POST ") + slack_url + " HTTP/1.1\r\n" +
                    "Host: " + IFTTT_host + "\r\n" + 
                    "Content-Type: application/json\r\n" + 
                    "Content-Length:" + jsonData.length() + "\r\n\r\n"
                    + jsonData);

        // String spotify_url = "/trigger/spotify_alert/with/key/";
        // spotify_url += IFTTT_apikey;

        // client.print(String("POST ") + spotify_url + " HTTP/1.1\r\n" +
        //             "Host: " + IFTTT_host + "\r\n" );
        lastTimeIFTTT = millis();
    } 
}

void openWeather() {
    if ((millis() - lastTimeHTTP) > timerDelayHTTP){
        if(WiFi.status()== WL_CONNECTED){
            jsonBuffer = httpGETRequest(openWeatherAPI.c_str());
            // Serial.println("jsonBuffer");
            // Serial.println(jsonBuffer);
            JSONVar myObject = JSON.parse(jsonBuffer);      
            // JSON.typeof(jsonVar) can be used to get the type of the var
            if (JSON.typeof(myObject) == "undefined") {
                Serial.println("Parsing input failed!");
                return;
            }
            local_temp = (double) myObject["main"]["temp"]- 273.15;
            local_temp_min = (double) myObject["main"]["temp_min"]- 273.15;
            local_temp_max = (double) myObject["main"]["temp_max"]- 273.15;
        }
        else {
            Serial.println("WiFi Disconnected");
        }
        lastTimeHTTP = millis();
    }
}

String httpGETRequest(const char* serverName) {
  WiFiClient client;
  HTTPClient http;
    
  // Your Domain name with URL path or IP address with path
  http.begin(client, serverName);
  
  // Send HTTP POST request
  int httpResponseCode = http.GET();
  
  String payload = "{}"; 
  
  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
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