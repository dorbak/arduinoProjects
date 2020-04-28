#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include "sensors.h"


const int REED_PIN = D5;

MySensors::MySensors()
{

}
    
bool MySensors::readReedSensor()
{
  int proximity = digitalRead(REED_PIN); // Read the state of the switch
  //  Serial.println(proximity);
  if (proximity == LOW) // If the pin reads low, the switch is closed.
  {
    return false;
  }
  else
  {
    return true;
  }
    return false;
}