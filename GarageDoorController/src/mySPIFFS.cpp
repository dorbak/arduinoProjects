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

String MySPIFFS::readFile(String filename)
{
    String results = "File not found";
    if (!SPIFFS.exists(filename))
    {
        Serial.println("File not found - ensure the file name is correct!");
        return "File not found";
    }
/*     File filehandle = SPIFFS.open(filename,"r");
    String results = filehandle.readStringUntil('\n');
    filehandle.close();
 */    return results;
}

bool MySPIFFS::writeToFile(File filehandle, String contents)
{
 /*  if (!SPIFFS.exists(filename))
  {
    Serial.println("File not open - you must call openFile() before calling writeToFile()");
    return false;
  } */
  
  //File handle = openFile(filename,"w");
  int bytesWritten = filehandle.print(contents);
  if (bytesWritten > 0)
  {
    return true;
  }
  return false;
}
