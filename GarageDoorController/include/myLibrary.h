#ifndef _myLibrary_H
    #define _myLibrary_H
#endif
#include <Arduino.h>
#include <FS.h>

class MyLibrary
{
public:
    MyLibrary();
    int myfunc(int val1, int val2);
    File openFile(String filename);
    boolean writeFile(File filehandle, String contents);
    void closeFile(File filehandle);
private: 

};
