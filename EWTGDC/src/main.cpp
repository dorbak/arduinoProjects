#include <Arduino.h>
#include "FS.h"
#include "ArduinoJson.h"
#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>

//Prototypes
String processor(const String &var);
String formatListOfNetworks();
void listFilesOnSPIFFS();
void launchInitialConfig();
void launchRegularServer();
void saveValues();
void saveValues2();

//Constants
bool oneTime = true;
const char wifiAPIdentifier[] = "EWTGDC";
//const char wifiAPPassword[] = "password";
const String wifiConfiguration = "/wifiConfiguration.cfg";
const String mqttConfiguration = "/mqttConfiguration.cfg";
const double maxTimeBetweenScans = 2500;

// Globals
AsyncWebServer server(80);

String wifiConnectionName, wifiConnectionPassword;
String mqtt_server, mqtt_server_port, mqtt_user = "unset";
String mqtt_password = "unset";

String availableNetworks;
double lastScanTime = 0;

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Starting up...");
  // Start FileSystem (SPIFFS)
  if (!SPIFFS.begin())
  {
    Serial.println("Panic - failed to initialize File System");
    return;
  }

  Serial.println("FileSystem loaded");

  listFilesOnSPIFFS();

  // Check to see if we have saved variables for our Wifi Connection
  // Look for WIFIConfiguration.txt file
  if (SPIFFS.exists(wifiConfiguration))
  {
    //  Serial.println("wifiConfiguration file found");
    File file = SPIFFS.open(wifiConfiguration, "r");
    // If Exists...
    // Load contents
    StaticJsonDocument<512> doc;
    // Deserialize the JSON document
    DeserializationError error = deserializeJson(doc, file);
    if (error)
    {
      Serial.println(F("Failed to read file, using default wifiConfiguration"));
      Serial.println(error.code());
    }
    wifiConnectionName = doc["wifiConnectionName"].as<String>();
    wifiConnectionPassword = doc["wifiConnectionPassword"].as<String>();
    //Serial.printf("SSID: %s\tPassword: %s\n",wifiConnectionName.c_str(), wifiConnectionPassword.c_str());
    file.close();

    // If yes, use them and connect
    Serial.printf("Attempting to connect to: %s", wifiConnectionName.c_str());
    WiFi.begin(wifiConnectionName, wifiConnectionPassword);
    int retryCount = 0;
    while (WiFi.status() != WL_CONNECTED && retryCount < 60)
    {

      Serial.print(".");
      delay(500);
      retryCount++;
    }
    if (WiFi.status() != WL_CONNECTED)
    {
      Serial.printf("Unable to connect to WiFi -- re-enabling SoftAP\n");
      if (SPIFFS.exists(wifiConfiguration))
      {
        SPIFFS.remove(wifiConfiguration);
        delay(500);
        Serial.printf("Restarting controller...\n");
        ESP.restart();
      }
    }
    Serial.print("\nConnected!  Local IP = ");
    Serial.println(WiFi.localIP());
    launchRegularServer();
  }
  else
  {
    //Serial.println("TODO: Start AP and scan for neighbouring WIFI SSIDs");
    WiFi.mode(WIFI_AP_STA);
    Serial.println(WiFi.softAP(wifiAPIdentifier) ? "Ready" : "Failed");
    Serial.print("AP IP Address: ");
    Serial.println(WiFi.softAPIP());
    launchInitialConfig();
  }

  //Load (if any) MQTT parameters
  if (SPIFFS.exists(mqttConfiguration))
  {
    Serial.println("Loading mqtt parameters");
    File file = SPIFFS.open(mqttConfiguration, "r");
    StaticJsonDocument<1024> doc;
    // Deserialize the JSON document
    DeserializationError error = deserializeJson(doc, file);
    if (error)
    {
      Serial.println(F("Failed to read file, using default wifiConfiguration"));
      Serial.println(error.code());
    }

    mqtt_server = doc["mqttServer"].as<String>();
    mqtt_server_port = doc["mqttServerPort"].as<String>();
    mqtt_user = doc["mqttUser"].as<String>();
    mqtt_password = doc["mqttPassword"].as<String>();
    Serial.printf("Server: %s\tPort: %s\tUsername: %s\tPassword: %s", mqtt_server.c_str(), mqtt_server_port.c_str(), mqtt_user.c_str(), mqtt_password.c_str());
    file.close();
  }
}
String scanAvailableNetworks()
{

  StaticJsonDocument<1000> doc;
  String result;

  int n = WiFi.scanComplete();
  //Serial.println("here");
  if (n == -2)
  {
    //Serial.println("There");
    WiFi.scanNetworks(true);
  }
  else if (n)
  {
    //Serial.println("WiFi network scanning complete");
    //Serial.printf("Number of networks found: %d", n);
    for (int i = 0; i < n; i++)
    {
      String ssid = WiFi.SSID(i);
      String tmp;
      serializeJsonPretty(doc, tmp);
      if (tmp.indexOf("\"" + ssid + "\"") < 0)
      {
        doc.add(ssid);
      }
    }

    int bytesWritten = serializeJson(doc, result);
    if (bytesWritten < 1)
    {
      Serial.println("Couldn't serialize the document :( ");
    }
    else
    {
      //Serial.println("oops");
    }
    WiFi.scanDelete();
    if (WiFi.scanComplete() == -2)
    {
      WiFi.scanNetworks(true);
    }
  }
  lastScanTime = millis();
  return result;
}

String processor(const String &var)
{
  //Serial.println("Entering processor()");
  if (var == "ListOfNetworks")
  {
    double _now = millis();
    //Serial.printf("Current time: %f\tLast Time: %f\tDiff: %f", _now, lastScanTime, _now - lastScanTime);
    if (_now - lastScanTime < maxTimeBetweenScans)
    {
      //Serial.println("too soon!! Returning last networks founds.");
      return formatListOfNetworks();
    }
    availableNetworks = scanAvailableNetworks();
    String html = formatListOfNetworks();
    //Serial.println("Leaving processor()");
    return html;
  }
  if (var == "mqttserver")
  {
    return mqtt_server;
  }
  if (var == "mqttport")
  {
    return mqtt_server_port;
  }
  if (var == "mqttuser")
  {
    return mqtt_user;
  }
  //Serial.println("Leaving processor()");
  return "Never made it in the IF statement - BAD";
}

String formatListOfNetworks()
{
  StaticJsonDocument<1000> doc;
  deserializeJson(doc, availableNetworks);
  String html;

  JsonArray array = doc.as<JsonArray>();

  for (int n = 0; n < int(doc.size()); n++)
  {
    String ssid = array[n].as<String>();
    html += "<input type=\"radio\" id=\"entry" + String(n) + "\" name=\"ssid\" value=\"" + ssid + "\">\n";
    html += "<label for=\"entry\">" + ssid + "</label><br>\n";
  }
  return html;
}

void launchInitialConfig()
{
  // Start WebServer
  Serial.println("Starting web server...");
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/style.css", "text/css");
  });
  server.on("/reset", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (SPIFFS.exists(mqttConfiguration) || SPIFFS.exists(wifiConfiguration))
    {
      Serial.printf("Resetting all settings - you'll need to reconfigure your controller");
      SPIFFS.remove(mqttConfiguration);
      SPIFFS.remove(wifiConfiguration);
      delay(500);
      ESP.restart();
    }
    request->redirect("/");
  });

  server.on("/submit", HTTP_POST, [](AsyncWebServerRequest *request) {
    int paramsNr = request->params();
    bool foundpassword = false;
    bool foundSSID = false;
    for (int i = 0; i < paramsNr; i++)
    {
      AsyncWebParameter *p = request->getParam(i);
      String paramName = p->name();

      if (paramName == "password")
      {
        wifiConnectionPassword = p->value();
        foundpassword = true;
      }
      else if (paramName == "ssid")
      {
        wifiConnectionName = p->value();
        foundSSID = true;
      }
    }
    if (foundSSID && foundpassword)
    {
      saveValues();
      //Serial.println("Found both!");
    }
    else
    {
      //Serial.println("frick");
    }
    request->redirect("/");
  });
  server.begin();
  Serial.println("Started.");
}

void launchRegularServer()
{
  Serial.println("Starting web server...");
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/main.html", String(), false, processor);
  });
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/style.css", "text/css");
  });
  server.on("/reset", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (SPIFFS.exists(mqttConfiguration) || SPIFFS.exists(wifiConfiguration))
    {
      Serial.printf("Resetting all settings - you'll need to reconfigure your controller");
      SPIFFS.remove(mqttConfiguration);
      SPIFFS.remove(wifiConfiguration);
      delay(500);
      ESP.restart();
    }
    request->redirect("/");
  });
  server.on("/submit", HTTP_POST, [](AsyncWebServerRequest *request) {
    int paramsNr = request->params();
    bool mqtt_server_found, mqtt_server_port_found, mqtt_user_found, mqtt_password_found = false;

    for (int i = 0; i < paramsNr; i++)
    {
      AsyncWebParameter *p = request->getParam(i);
      String paramName = p->name();

      if (paramName == "mqtt_server")
      {
        mqtt_server = p->value();
        mqtt_server_found = true;
      }
      else if (paramName == "mqtt_server_port")
      {
        mqtt_server_port = p->value();
        mqtt_server_port_found = true;
      }
      if (paramName == "mqtt_user")
      {
        mqtt_user = p->value();
        mqtt_user_found = true;
      }
      else if (paramName == "mqtt_password")
      {
        Serial.println(p->value());
        mqtt_password = p->value();
        mqtt_password_found = true;
      }
    }
    if (mqtt_server_found && mqtt_server_port_found && mqtt_user_found && mqtt_password_found)
    {
      saveValues2();
      //Serial.println("Found both!");
    }
    else
    {
      //Serial.println("frick");
    }
    request->redirect("/");
  });
  server.begin();
  Serial.println("Started.");
}
void saveValues()
{
  StaticJsonDocument<100> doc;
  File configFile = SPIFFS.open(wifiConfiguration, "w");
  // Set the values in the document

  doc["wifiConnectionName"] = wifiConnectionName;
  doc["wifiConnectionPassword"] = wifiConnectionPassword;
  if (serializeJson(doc, configFile) == 0)
  {
    Serial.println(F("Failed to write to file"));
  }
  configFile.close();
  Serial.println("Wifi wifiConfiguration saved.  Rebooting");
  delay(500);
  ESP.restart();
  return;
}
void saveValues2()
{
  StaticJsonDocument<250> doc;
  File configFile = SPIFFS.open(mqttConfiguration, "w");
  // Set the values in the document

  doc["mqttServer"] = mqtt_server;
  doc["mqttServerPort"] = mqtt_server_port;
  doc["mqttUser"] = mqtt_user;
  doc["mqttPassword"] = mqtt_password;
  if (serializeJson(doc, configFile) == 0)
  {
    Serial.println(F("Failed to write to file"));
  }
  configFile.close();
  Serial.println("MQTT Configuration saved.");
  return;
}
void listFilesOnSPIFFS()
{
  Serial.println("Displaying all files found");
  Dir dir = SPIFFS.openDir("/");
  while (dir.next())
  {
    String str;
    str += dir.fileName();
    str += " / ";
    str += dir.fileSize();
    Serial.println(str);
  }
}
void loop()
{
  while (oneTime)
  {
    //  availableNetworks = scanAvailableNetworks();
    oneTime = false;
  }
  // put your main code here, to run repeatedly:
  // String json_Networks = scanAvailableNetworks();
}