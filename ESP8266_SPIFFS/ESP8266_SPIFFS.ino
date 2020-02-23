/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

#ifdef ESP32
#pragma message(THIS EXAMPLE IS FOR ESP8266 ONLY!)
#error Select ESP8266 board.
#endif

// Set LED GPIO
const int ledPin = 2;
// Stores LED state
String ledState;

#ifndef File
  #include <FS.h>
#endif

extern File file;

void setup() {
  // Serial port for debugging purposes
  Serial.begin(115200);
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
   readDHT();
   if (!file)
   {
    file = FileOpen("test.txt","r",false);
   }
}
