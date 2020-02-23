#include <Arduino.h>
#include <myWifi.h>
#include <myDHT.h>
#include <ESPAsyncWebServer.h>

MyDHT myDHT2;
AsyncWebServer server(80);

MyWifi localInstance;

char* ssid = "Loki";
char* password = "Pelletier";

// Set LED GPIO
const int ledPin = 2;
// Stores LED state
String ledState2;
MyWifi::MyWifi()
{
    
}

// Replaces placeholder with LED state value
String MyWifi::processor(const String& var) {
  Serial.println(var);
  if (var == "STATE") {
    if (digitalRead(ledPin)) {
      ledState2 = "OFF";
    }
    else {
      ledState2 = "ON";
    }
    Serial.print(ledState2);
    return ledState2;
  }
  else if (var == "TEMPERATURE") {
    return myDHT2.getTemp();
  }
  else if (var == "HUMIDITY") {
    return myDHT2.getHum();
  }
  else if (var == "PRESSURE") {
    return "N/A";
  }
  return "Error!";
}
void MyWifi::setupWifi()
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

void MyWifi::setupServer()
{
    
   // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/index.html", String(), false, localInstance.processor);
  });

  // Route to load style.css file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/style.css", "text/css");
  });

  // Route to set GPIO to HIGH
  server.on("/on", HTTP_GET, [](AsyncWebServerRequest * request) {
    digitalWrite(ledPin, HIGH);
    request->send(SPIFFS, "/index.html", String(), false, localInstance.processor);
  });

  // Route to set GPIO to LOW
  server.on("/off", HTTP_GET, [](AsyncWebServerRequest * request) {
    digitalWrite(ledPin, LOW);
    request->send(SPIFFS, "/index.html", String(), false, localInstance.processor);
  });

  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", String(myDHT2.getTemp()).c_str(), localInstance.processor);
  });

  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", String(myDHT2.getHum()).c_str(), localInstance.processor);
  });

  server.on("/pressure", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", String(myDHT2.getMyPressure()).c_str(), localInstance.processor);
  });

  // Start server
  server.begin();
}