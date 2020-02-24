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

  int test = mylib.myfunc(rand()*10,rand()*10);
  
  Serial.println("Writing to file...");
  File filehandle = mySpiffs.openFile("test.txt","w");
  if (mylib.writeFile(filehandle,String(test)))
  {
    Serial.println("Successfully written the value to file...");
    Serial.print("Value written: ");
    Serial.println(test);
  }

  myDHT.readDHT();
}