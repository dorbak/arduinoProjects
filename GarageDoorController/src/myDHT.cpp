#include <Arduino.h>
#include <myDHT.h>
#include <DHT.h>
#include <Adafruit_Sensor.h>

#define DHTPin 4 // D2
#define DHTTYPE DHT11
DHT dht(DHTPin, DHTTYPE);
float h,t,f;
MyDHT::MyDHT()
{
     
}
void MyDHT::setupDHT()
{
   // DHT Sensor setup
  Serial.println("Entering sensor Setup");

  dht.begin();
}

String MyDHT::getMyPressure() {
  return "N/A";
}
String MyDHT::getTemp() {
  return String(t);
}
String MyDHT::getHum() {
  return String(h);
}

void MyDHT::readDHT()
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