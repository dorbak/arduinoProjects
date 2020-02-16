
#include "EspMQTTClient.h"
#include "ArduinoJson.h"


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


void setup()
{
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  
  // Initialize SPIFFS
  if(!SPIFFS.begin(true))
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  
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

void publishData(String buffer) 
{
  //Serial.println(buffer);
   client.publish("garage/status", "test");
}

void readSensor(String buffer)
{
    String garageSensor = "";
    int proximity = digitalRead(REED_PIN); // Read the state of the switch
    //  Serial.println(proximity);
    if (proximity == LOW) // If the pin reads low, the switch is closed.
    {
      garageSensor = "Open";
    }
    else
    {
      garageSensor = "Closed";
    }
    client.publish("garage/status", garageSensor);    


//    // create a JSON object
//    // doc : https://github.com/bblanchon/ArduinoJson/wiki/API%20Reference
//    StaticJsonDocument<200> doc;
//    doc["doorSensor"] = garageSensor ;
//    serializeJson(doc, Serial);
//    //char buffer[512];
//    size_t n = serializeJson(doc, buffer);

}

void loop()
{
  while(!Serial) {} // Wait for the serial to intialize from wake/boot

  String dataToSend;
  readSensor(dataToSend);
  
//  Serial.println(dataToSend);
//  client.publish("garage/status", dataToSend);
  
  //publishData(dataToSend);

  delay(500);
  

  client.loop();

}
