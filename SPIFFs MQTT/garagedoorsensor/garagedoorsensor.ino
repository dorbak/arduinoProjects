
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <FS.h>   // Include the SPIFFS library

#include "EspMQTTClient.h"
#include "ArduinoJson.h"

ESP8266WiFiMulti wifiMulti;     // Create an instance of the ESP8266WiFiMulti class, called 'wifiMulti'
ESP8266WebServer server(80);    // Create a webserver object that listens for HTTP request on port 80

String getContentType(String filename); // convert the file extension to the MIME type
bool handleFileRead(String path);       // send the right file to the client (if it exists)

String garageSensor = "";

const int REED_PIN = D0; // Pin connected to reed switch


EspMQTTClient client(
  "Loki",
  "Pelletier",
  "192.168.1.235",  // MQTT Broker server ip
  "eric",   // Can be omitted if not needed
  "TY53jkPw2a",   // Can be omitted if not needed
  "Arduino_testClient",     // Client name that uniquely identify your device
  1883              // The MQTT port, default to 1883. this line can be omitted
);

// Replaces placeholder with LED state value
String processor(const String& var) {

  if (var == "STATE") {
    if (garageSensor == "")
    {
      int proximity = digitalRead(REED_PIN); // Read the state of the switch
      if (proximity == LOW) // If the pin reads low, the switch is closed.
      {
        garageSensor = "Closed";
      }
      else
      {
        garageSensor = "Opened";
      }
    }
    return garageSensor;
  }
  return String();
}

void setup()
{
  Serial.begin(9600);
  delay(10);

  Serial.println('\n');
  wifiMulti.addAP("Loki", "Pelletier");   // add Wi-Fi networks you want to connect to

  Serial.println('\n');
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());              // Tell us what network we're connected to
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());           // Send the IP address of the ESP8266 to the computer

  if (MDNS.begin("esp8266")) {              // Start the mDNS responder for esp8266.local
    Serial.println("mDNS responder started");
  } else {
    Serial.println("Error setting up MDNS responder!");
  }

  SPIFFS.begin();                           // Start the SPI Flash Files System

  server.onNotFound([]() {                              // If the client requests any URI
    if (!handleFileRead(server.uri()))                  // send it if it exists
      server.send(404, "text/plain", "404: Not Found"); // otherwise, respond with a 404 (Not Found) error
  });

  server.begin();                           // Actually start the server
  Serial.println("HTTP server started");
}

// This function is called once everything is connected (Wifi and MQTT)
// WARNING : YOU MUST IMPLEMENT IT IF YOU USE EspMQTTClient
void onConnectionEstablished()
{
  // Subscribe to "mytopic/test" and display received message to Serial
  //client.subscribe("mytopic/test", [](const String & payload) {
  //  Serial.println(payload);
  //});
  Serial.println("Connected!");
  return;
}

void readSensor(String buffer)
{

  int proximity = digitalRead(REED_PIN); // Read the state of the switch
  //  Serial.println(proximity);
  if (proximity == LOW) // If the pin reads low, the switch is closed.
  {
    garageSensor = "Closed";
  }
  else
  {
    garageSensor = "Opened";
  }
  client.publish("garage/status", garageSensor);
}

void loop()
{
  while (!Serial) {} // Wait for the serial to intialize from wake/boot
  server.handleClient();
  String dataToSend;
  readSensor(dataToSend);

  delay(500);


  client.loop();

}

String getContentType(String filename) { // convert the file extension to the MIME type
  if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  return "text/plain";
}

bool handleFileRead(String path) { // send the right file to the client (if it exists)
  Serial.println("handleFileRead: " + path);
  if (path.endsWith("/")) path += "index.html";         // If a folder is requested, send the index file
  String contentType = getContentType(path);            // Get the MIME type
  if (SPIFFS.exists(path)) {                            // If the file exists
    File file = SPIFFS.open(path, "r");                 // Open it
    size_t sent = server.streamFile(file, contentType); // And send it to the client
    file.close();                                       // Then close the file again
    return true;
  }
  Serial.println("\tFile Not Found");
  return false;                                         // If the file doesn't exist, return false
}
