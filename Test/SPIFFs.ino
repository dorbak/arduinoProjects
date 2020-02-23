#ifndef FS_H
  #include <FS.h>
#endif
#ifndef File
  #include <FS.h>
#endif
File file;

File FileOpen(String filename, String fileMode = "r", bool createFile = true)
{
  String fileToOpen = "/" + filename;
  Serial.println(fileToOpen);
  if (SPIFFS.exists(fileToOpen) && !createFile)
  {
    Serial.println("File not found --- createFile set to FALSE");
    return NULL;
  }
  file = SPIFFS.open(fileToOpen, fileMode.c_str());
  if (!file)
  {
    Serial.println("Error opening or creating the file... :(");
    return NULL;
  }
  return file;
}
void setupSPIFFS() {
  if (!SPIFFS.begin()) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
}

File createFile(String filename, bool overwrite = false)
{
  if (SPIFFS.exists(filename) && !overwrite)
  {
    Serial.println("File Exists and overwrite set to false");
    return false;
  }
}
