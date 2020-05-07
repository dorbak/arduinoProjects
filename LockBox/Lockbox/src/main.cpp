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
const size_t capacity = JSON_ARRAY_SIZE(0) + 2*JSON_ARRAY_SIZE(3) + JSON_OBJECT_SIZE(2) + 4*JSON_OBJECT_SIZE(4) + 4*JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(7) + JSON_OBJECT_SIZE(22) + JSON_OBJECT_SIZE(85) + 2270;
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
bool firstLoop = true;
char currentTime[10];
char currentDay[12];
long sessionStart;
long sessionEnd;
long lockTimeInLock;
String sessionID;

AsyncWebServer server(80);

String header;
String lockStartTime;
String lockEndTime = "EndTime";
String lockStatus = "LockStatus";
String lockTimeLeft;

char* json;
String payload;

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
String convertEpochToDateTime(long _epoch);

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
    const uint8_t fingerprint[20] = {0x1C, 0x2B, 0xC2, 0x8B, 0x01, 0x78, 0xA3, 0x6C, 0xA9, 0xF4, 0xAA, 0x15, 0x5F, 0x3D, 0xA0, 0xBD, 0x69, 0x88, 0x54, 0x9E};
    std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
    client->setFingerprint(fingerprint);
    
    // Send request
    //http.useHTTP10(true);
    HTTPClient https;
    
    if (https.begin(*client, "https://www.emlalock.com/info/?userid=jlpgkzrtnw2y5cw&apikey=s6h5gfxjuf"))
    {
      int httpCode = https.GET();
      if (httpCode > 0) {
        Serial.printf("[HTTPS] GET ... code: %d\n", httpCode);
      }
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY)
      {
        payload = https.getString();
        //Serial.println(payload);
      }
      else
      {
        Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
      }
    }
    else
    {
      Serial.printf("[HTTPS] Unable to connect\n");
    }
      // Disconnect
    https.end();
  
}
void setupWebServer()
{
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    getLockStatus();
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
    return convertEpochToDateTime(sessionEnd);
  }
  if(var == "lockStartTime"){
    return convertEpochToDateTime(sessionStart);
  }
  if(var == "totalLockTime"){
    timeClient.update();
    long rightNow = timeClient.getEpochTime();
    return convertEpochToDateTime(rightNow-sessionStart);
  }

  return String();
}

String convertEpochToDateTime(long thisEpoch)
{
  int _currentDay, _currentMonth, _currentYear, _currentHour, _currentMinute, _currentSecond;
  struct tm *ptm = gmtime ((time_t *)&thisEpoch); 
  _currentDay = ptm->tm_mday;
  _currentMonth = ptm->tm_mon+1;
  _currentYear = ptm->tm_year+1900;
  _currentHour = ptm->tm_hour;
  _currentMinute = ptm->tm_min;
  _currentSecond = ptm->tm_sec;
  if (ptm->tm_isdst == 0)
  {
    _currentHour = _currentHour + 1;
  }

  char _datetime[24];
  sprintf(_datetime,"%04u-%02u-%02u  %02u:%02u:%02u", _currentYear, _currentMonth, _currentDay, _currentHour, _currentMinute, _currentSecond);
  return _datetime;
}

void loop() {
  // put your main code here, to run repeatedly:
 
  if (firstLoop)
  {
    //Serial.println(payload);
    // Parse response
    DynamicJsonDocument doc(capacity);
    deserializeJson(doc, payload);
    JsonObject user = doc["user"];
    JsonObject sessions = doc["chastitysession"];
    String user_userid;
    user_userid = user["userid"].as<String>();
    sessionID = sessions["chastitysessionid"].as<String>();
    sessionStart = sessions["startdate"].as<long>();
    sessionEnd = sessions["enddate"].as<long>();
    lockTimeInLock = sessions["timeinlock"].as<long>();
    // Serial.println(user_userid);
    Serial.println(user_userid);
    Serial.println(lockTimeInLock);
    // yield();
    firstLoop = false;
  }

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
