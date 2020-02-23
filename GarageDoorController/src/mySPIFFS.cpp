#include <Arduino.h>
#include <MySPIFFS.h>

MySPIFFS::MySPIFFS()
{

}



void MySPIFFS::setupSPIFFS() {
  if (!SPIFFS.begin()) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
}

File MySPIFFS::openFile(String filename, String filemode = "r")
{
  File file = SPIFFS.open(filename, filemode.c_str());
  return file;
}

bool MySPIFFS::writeToFile(File file, String contents)
{
  if (!file)
    Serial.println("File not open - you must call openFile() before calling writeToFile()");
  int bytesWritten = file.print(contents);
  if (bytesWritten > 0)
    return true;
  return false;
}
