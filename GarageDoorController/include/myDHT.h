#ifndef _myDHT_H
#define _myDHT_H
#endif
#include <Arduino.h>
class MyDHT
{
public:
    MyDHT();
    void setupDHT();
    String getMyPressure();
    String getTemp();
    String getHum();
    void readDHT();

private:
  
};
