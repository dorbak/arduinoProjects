#include <Arduino.h>
#include "myLibrary.h"
#include <mySPIFFS.h>
#include <FS.h>

MyLibrary::MyLibrary()
{

}

File myFile;
MySPIFFS myspiffs;

int MyLibrary::myfunc(int val1, int val2)
{
    return val1+val2;
}
File MyLibrary::openFile(String filename)
{
    myFile = myspiffs.openFile("test.txt","w");
    return myFile;
}
boolean MyLibrary::writeFile(File filehandle, String contents)
{
    bool results = myspiffs.writeToFile(filehandle, contents);
    return results;
}
void MyLibrary::closeFile(File filehandle)
{
    filehandle.close();
}