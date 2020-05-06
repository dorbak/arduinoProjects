#include <Arduino.h>

#include "ESPAsyncWebServer.h"
#include "FS.h"
#include <Servo.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "ArduinoJson.h"
#include "ESP8266HTTPClient.h"

const char* ssid = "Loki";
const char* password = "Pelletier";
const int locked = 10;
const int unlocked = 65;

const String emlaAPIUser = "jlpgkzrtnw2y5cw";
const String elmaAPIkey = "s6h5gfxjuf";

const int hours = 8;

long timeout = 8 * 60 * 60;

long epochTime;

int monthDay;
int currentMonth;
int currentYear;
int currentHour;
int currentMinute;
int currentSecond;
int secondsSinceNTPGet = 0;
char currentTime[10];
char currentDay[12];

AsyncWebServer server(80);

String header;
String lockStartTime;
String lockEndTime = "EndTime";
String lockStatus = "LockStatus";
String lockTimeLeft;

char* json;

// NTP Stuff
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "ca.pool.ntp.org");

int servoPin = D5;
Servo myServo;

String processor(const String &var);
void updateDateTime();
void lockIt();
void unlockIt();
void formatDateTime();
void setupWebServer();
void getLockStatus();

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);
  while (!Serial)
  {
    delay(100);
  }

  // Start FileSystem (SPIFFS)
  if (!SPIFFS.begin())
  {
    Serial.println("Panic - failed to initialize File System");
    return;
  }

  Serial.println("FileSystem loaded");

  myServo.attach(servoPin);

  Serial.print("Connecting to: ");
  Serial.println(ssid);
  WiFi.begin(ssid,password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(">");
  }
  Serial.println("");
  Serial.println("Wifi Connected.");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  
  // Start the NTP Client
  timeClient.begin();
  // Get the current time
  updateDateTime();

  // Start WebServer Section
  // Route for root / web page
  setupWebServer();
  
  // Todo:  Display Current Time
  // Todo:  Display Open Time
  // Todo:  Enter code 

  timeClient.update();
  timeClient.setTimeOffset(-7*3600);
  updateDateTime();
  formatDateTime();
  getLockStatus();
  // Start server
  server.begin();
}
void getLockStatus()
{
    const size_t bufferSize = JSON_ARRAY_SIZE(0) + 2*JSON_ARRAY_SIZE(3) + JSON_OBJECT_SIZE(2) + 4*JSON_OBJECT_SIZE(4) + 4*JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(7) + JSON_OBJECT_SIZE(22) + JSON_OBJECT_SIZE(85) + 2270;
    HTTPClient http;

    // Send request
    http.useHTTP10(true);
    http.begin("https://www.emlalock.com/info/?userid=jlpgkzrtnw2y5cw&apikey=s6h5gfxjuf");
    http.GET();

    Serial.println(http.getStream());

    /* // Parse response
    DynamicJsonDocument doc(bufferSize);
    deserializeJson(doc, http.getStream());

    // Read values
    JsonObject user = doc["user"];
    Serial.println(doc.as<String>());
 */
    // Disconnect
    http.end();
  
}
void setupWebServer()
{
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/control.html", String(), false, processor);
  });
  
  // Route to load style.css file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/style.css", "text/css");
  });

  // Route to lock
  server.on("/lock", HTTP_GET, [](AsyncWebServerRequest *request){
  lockIt();
  request->send(SPIFFS, "/control.html", String(), false, processor);
  });
  
  // Route to unlock
  server.on("/unlock", HTTP_GET, [](AsyncWebServerRequest *request){
    unlockIt();
   
    request->send(SPIFFS, "/control.html", String(), false, processor);
  });
}

void lockIt()
{
  lockStatus="Locked!";
  myServo.write(locked);
  
}
void unlockIt()
{
  
  lockStatus = "Unlocked!";
  myServo.write(unlocked);
  
}

// Replaces placeholder with variable values
String processor(const String& var){
  if(var == "lockStatus"){
    return lockStatus;
  }
  if(var == "lockEndTime"){
    return lockEndTime;
  }
  if(var == "lockStartTime"){
    return String(currentDay) + " " + String(currentTime);
  }
  if(var == "lockTimeLeft"){
    return lockTimeLeft;
  }
  return String();
}
 
void loop() {
  // put your main code here, to run repeatedly:
 
  secondsSinceNTPGet = secondsSinceNTPGet + 1;
  if (secondsSinceNTPGet > 60)
  {
    Serial.println("Forcing a time update!");
    secondsSinceNTPGet = 0;
    updateDateTime();
  }
  else
  {
    currentSecond += 1;
    if (currentSecond == 60)
    {
      currentSecond = 0;
      currentMinute += 1;
      if (currentMinute > 60)
      {
        currentMinute = 0;
        currentHour += 1;
        if (currentHour == 24)
        {
          // Force a new time/date get
          Serial.println("Forcing a time update!");
          updateDateTime();
        }
      }
    }
  }
  formatDateTime();
  delay(1000);
}

void formatDateTime()
{
  sprintf (currentTime,"%02u:%02u:%02u",currentHour, currentMinute, currentSecond);
  sprintf (currentDay,"%04u-%02u-%02u",currentYear, currentMonth, monthDay);
}

void updateDateTime()
{
    timeClient.update();
    epochTime = timeClient.getEpochTime();

    struct tm *ptm = gmtime ((time_t *)&epochTime); 
    monthDay = ptm->tm_mday;
    currentMonth = ptm->tm_mon+1;
    currentYear = ptm->tm_year+1900;
    currentHour = timeClient.getHours();
    currentMinute = timeClient.getMinutes();
    currentSecond = timeClient.getSeconds();
    if (ptm->tm_isdst == 0)
    {
      currentHour = currentHour + 1;
    }
    secondsSinceNTPGet = 0;
}
