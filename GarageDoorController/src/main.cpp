#include <Arduino.h>
#include <myDHT.h>
#include <myLibrary.h>
#include <mySPIFFS.h>
#include <myWifi.h>



// Set LED GPIO
const int ledPin = 2;
// Stores LED state
String ledState;

MyLibrary mylib;
MyDHT myDHT;
MySPIFFS mySpiffs;
MyWifi myWifi;




void setup() {
   // Serial port for debugging purposes
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  // Setup Wifi
  myWifi.setupWifi();
  // Setup DHT Sensor
  myDHT.setupDHT();
  // Initialize SPIFFS
  mySpiffs.setupSPIFFS();
  // setup WebServer
  myWifi.setupServer();

}


void loop() {
  // put your main code here, to run repeatedly:

  Serial.println(mylib.myfunc(1,3));
  myDHT.readDHT();
}