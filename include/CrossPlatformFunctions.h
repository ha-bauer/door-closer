#ifndef CROSS_PLATFORM_FUNCTIONS_H
#define CROSS_PLATFORM_FUNCTIONS_H


#ifdef ARDUINO
    #include <RTClib.h>
    #define INT_TO_STR(int) String(int)
    #define PRINT(str) Serial.print(str)
    #define C_STR(str) str.c_str()
#else
    #include <iostream>
    #include <sstream>
    #include <string>
    
    #include "DateTime.h"
    #include "TimeSpan.h"
    
    #define String std::string
    #define INT_TO_STR(value) to_string(value)
    #define PRINT(str) std::cout << str
    #define C_STR(str) str.substr(0, str.size()).c_str()
    
    using namespace DateTimeUnitTesting;

    std::string to_string(long value);
#endif

String toString(DateTime dt);

#endif