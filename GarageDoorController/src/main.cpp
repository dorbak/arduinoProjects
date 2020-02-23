#include <Arduino.h>
#include <FS.h>
#include <ESPAsyncWebServer.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

// Set LED GPIO
const int ledPin = 2;
// Stores LED state
String ledState;
#define DHTPin 4 // D2
#define DHTTYPE DHT11

char* ssid = "Loki";
char* password = "Pelletier";
AsyncWebServer server(80);
float h,t,f;


DHT dht(DHTPin, DHTTYPE);


String getMyPressure() {
  return "N/A";
}
String getTemp() {
  return String(t);
}
String getHum() {
  return String(h);
}
// Replaces placeholder with LED state value
String processor(const String& var) {
  Serial.println(var);
  if (var == "STATE") {
    if (digitalRead(ledPin)) {
      ledState = "ON";
    }
    else {
      ledState = "OFF";
    }
    Serial.print(ledState);
    return ledState;
  }
  else if (var == "TEMPERATURE") {
    return getTemp();
  }
  else if (var == "HUMIDITY") {
    return getHum();
  }
  else if (var == "PRESSURE") {
    return "N/A";
  }
  return "Error!";
}


void setupServer()
{
   // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });

  // Route to load style.css file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/style.css", "text/css");
  });

  // Route to set GPIO to HIGH
  server.on("/on", HTTP_GET, [](AsyncWebServerRequest * request) {
    digitalWrite(ledPin, HIGH);
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });

  // Route to set GPIO to LOW
  server.on("/off", HTTP_GET, [](AsyncWebServerRequest * request) {
    digitalWrite(ledPin, LOW);
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });

  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", String(getTemp()).c_str(), processor);
  });

  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", String(getHum()).c_str(), processor);
  });

  server.on("/pressure", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", String(getMyPressure()).c_str(), processor);
  });

  // Start server
  server.begin();
}

void setupDHT()
{
   // DHT Sensor setup
  Serial.println("Entering sensor Setup");

  dht.begin();
}
void setupSPIFFS() {
  if (!SPIFFS.begin()) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
}
void setupWifi()
{
   // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
    // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());
}

File openFile(String filename, String filemode = "r")
{
  File file = SPIFFS.open(filename, filemode.c_str());
  return file;
}

bool writeToFile(File file, String contents)
{
  if (!file)
    Serial.println("File not open - you must call openFile() before calling writeToFile()");
  int bytesWritten = file.print(contents);
  if (bytesWritten > 0)
    return true;
  return false;
}


void readDHT()
{
  // Wait a few seconds between measurements.
  delay(2000);
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);
  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
  Serial.print(f);
  Serial.print(F("°F  Heat index: "));
  Serial.print(hic);
  Serial.print(F("°C "));
  Serial.print(hif);
  Serial.println(F("°F"));
}


void setup() {
   // Serial port for debugging purposes
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  
  // Setup Wifi
  setupWifi();
  // Setup DHT Sensor
  setupDHT();
  // Initialize SPIFFS
  setupSPIFFS();
  // setup WebServer
  setupServer();

}


void loop() {
  // put your main code here, to run repeatedly:
  readDHT();
}