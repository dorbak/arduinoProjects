#include <FS.h> //this needs to be first, or it all crashes and burns...

#include <ESP8266WiFi.h> //https://github.com/esp8266/Arduino

//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h> //https://github.com/tzapu/WiFiManager
//#include <ESPAsyncWebServer.h>

ESP8266WebServer server(80);

bool loadFromSpiffs(String path)
{
  String dataType = "text/plain";
  if (path.endsWith("/")) path += "index.html"; //this is where index.htm is created

  if (path.endsWith(".src")) path = path.substring(0, path.lastIndexOf("."));
  else if (path.endsWith(".htm")) dataType = "text/html";
  else if (path.endsWith(".html")) dataType = "text/html";
  else if (path.endsWith(".css")) dataType = "text/css";
  else if (path.endsWith(".js")) dataType = "application/javascript";
  else if (path.endsWith(".png")) dataType = "image/png";
  else if (path.endsWith(".gif")) dataType = "image/gif";
  else if (path.endsWith(".jpg")) dataType = "image/jpeg";
  else if (path.endsWith(".ico")) dataType = "image/x-icon";
  else if (path.endsWith(".xml")) dataType = "text/xml";
  else if (path.endsWith(".pdf")) dataType = "application/pdf";
  else if (path.endsWith(".zip")) dataType = "application/zip";

  File dataFile = SPIFFS.open(path.c_str(), "r");   //open file to read
  if (!dataFile)  //unsuccesful open
  {
    Serial.print("Don't know this command and it's not a file in SPIFFS : ");
    Serial.println(path);
    return false;
  }
  if (server.hasArg("download")) dataType = "application/octet-stream";
  if (server.streamFile(dataFile, dataType) != dataFile.size()) {}    //a lot happening here

  dataFile.close();

  return true; //shouldn't always return true, Added false above
}
void handleOther() {   
  
  if (loadFromSpiffs(server.uri())) return;   //gotcha - it's a file in SPIFFS
  String message = "Not Found\n\n";           //or not...
  message += "URI: ";     //make a 404 response & provide debug information
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++)
  {
    message += " NAME:" + server.argName(i) + "\n VALUE:" + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  Serial.println(message);
}


void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println();

  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;

  //exit after config instead of connecting
  wifiManager.setBreakAfterConfig(true);

  //reset settings - for testing
  //wifiManager.resetSettings();

  //tries to connect to last known settings
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP" with password "password"
  //and goes into a blocking loop awaiting configuration
  if (!wifiManager.autoConnect("AutoConnectAP", "password"))
  {
    Serial.println("failed to connect, we should reset as see if it connects");
    delay(3000);
    ESP.reset();
    delay(5000);
  }

  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");

  Serial.println("local ip");
  Serial.println(WiFi.localIP());

  if (WiFi.status() == WL_CONNECTED) // Start the ASyncWebServer
  {
    if (!SPIFFS.begin())
    {
      Serial.println("An error occurred mounting the SPIFFS file system");
      return;
    }

    server.onNotFound(handleOther);
    server.begin();
    Serial.print("Web server started on ");
    Serial.println(WiFi.localIP());
  }
}

void loop()
{
  // put your main code here, to run repeatedly:
  if (WiFi.status() == WL_CONNECTED) // Start the ASyncWebServer
  {
   server.handleClient();
  }
}
