#include <Arduino.h>
#include "myLibrary.h"

MyLibrary::MyLibrary()
{

}

int MyLibrary::myfunc(int val1, int val2)
{
    return val1+val2;
}