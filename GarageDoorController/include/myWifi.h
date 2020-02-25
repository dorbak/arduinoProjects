#ifndef _myWifi_H
#define _myWifi_H
#endif
#include <Arduino.h>
class MyWifi
{
public:
    MyWifi();
    
    void setupServer();
    void setupWifi(char* ssid, char* password);  
    void setupAP();

private:
static String processor(const String& var);
};
