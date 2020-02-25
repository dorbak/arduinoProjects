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


void MyWifi::setupAP()
{
  
  IPAddress local_IP(192,168,1,1);
  IPAddress gateway(192,168,1,1);
  IPAddress subnet(255,255,255,0);

  Serial.begin(9600);
  Serial.println();
  Serial.print("Setting soft-AP configuration ... ");
  Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready" : "Failed!");
  Serial.print("Setting soft-AP ... ");
  Serial.println(WiFi.softAP("EWT_GarageController") ? "Ready" : "Failed!");
  Serial.print("Soft-AP IP address = ");
  Serial.println(WiFi.softAPIP());


  
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
void MyWifi::setupWifi(char* ssid, char* password)
{

// CHECK OUT!!!!:  https://randomnerdtutorials.com/wifimanager-with-esp8266-autoconnect-custom-parameter-and-manage-your-ssid-and-password/

  Serial.println("Disconnecting form Wifi AP");
  
  // Switch to Station Mode
  WiFi.mode((WiFiMode)WIFI_STA);
  WiFi.config(IPAddress(0,0,0,0),IPAddress(0,0,0,0),IPAddress(0,0,0,0)); // Reset to use DHCP
  
  // Connect to Wi-Fi
  Serial.printf("Attempting to connect to: %s\n",ssid); 
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(0);
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
 // Route for root / web page
  server.on("/setup", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/configure.html");
  });
  // Route to try to connect to WIFI AP entered by user
  server.on("/connect", HTTP_GET, [](AsyncWebServerRequest *request) {
    int paramsNr = request->params();
    Serial.print("Number of parameters received: ");
    Serial.println(paramsNr);
    String APInfo[2];
    
    for (int i = 0; i < paramsNr; i++)
    {
      AsyncWebParameter *p = request->getParam(i);
      Serial.print(p->name());
      Serial.print(": ");
      Serial.println(p->value());
      Serial.println("------");
      APInfo[i] = p->value();
    }
    
    Serial.println("Trying to connect to AP");
    Serial.println("Disconnecting from Wifi AP");
    WiFi.softAPdisconnect(true);
    Serial.println("Pausing 2s");

    delay(2000);
    
    Serial.println("resuming...");
    // Connect to Wi-Fi
    Serial.printf("Attempting to connect to: %s\n",APInfo[0].c_str()); 
    char ssid[APInfo[0].length()+1];
    strcpy(ssid, APInfo[0].c_str());
    char pwd[APInfo[1].length()+1];

    localInstance.setupWifi(ssid,pwd);

    request->redirect("/setup");
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