#ifndef _mySPIFFS_H
#define _mySPIFFS_H
#endif
#include <Arduino.h>
#include <FS.h>
class MySPIFFS
{
public:
    MySPIFFS();
    bool writeToFile(File filehandle, String contents);
    File openFile(String filename, String filemode);
    void setupSPIFFS();
    String readFile(String filename);
private:
  
};
