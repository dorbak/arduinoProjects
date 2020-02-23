#ifndef DEBUG_WIFI
  #include <ESP8266WiFi.h>
#endif
#ifndef _ESPAsyncWebServer_H_
  #include <ESPAsyncWebServer.h>
#endif  
#ifndef ASYNCTCP_H_
  #include <ESPAsyncTCP.h>
#endif

IPAddress local_IP(10,10,10,10);
IPAddress gateway(10,10,10,1);
IPAddress subnet(255,255,255,0);
char* ssid = "Loki";
char* password = "Pelletier";
AsyncWebServer server(80);

// Create AsyncWebServer object on port 80
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


void setupAP()
{
  Serial.begin(115200);
  Serial.println();
  Serial.print("Setting soft-AP configuration ... ");
  Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready" : "Failed!");
  Serial.print("Setting soft-AP ... ");
  Serial.println(WiFi.softAP("ESPsoftAP_01") ? "Ready" : "Failed!");
  Serial.print("Soft-AP IP address = ");
  Serial.println(WiFi.softAPIP());
}
void wifiLoop()
{
  
}
