#ifndef _mySPIFFS_H
#define _mySPIFFS_H
#endif
#include <Arduino.h>
#include <FS.h>
class MySPIFFS
{
public:
    MySPIFFS();
    bool writeToFile(File file, String contents);
    File openFile(String filename, String filemode);
    void setupSPIFFS();

private:
  
};
