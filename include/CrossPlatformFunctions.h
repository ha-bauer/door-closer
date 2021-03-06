#ifndef CROSS_PLATFORM_FUNCTIONS_H
#define CROSS_PLATFORM_FUNCTIONS_H


#ifdef ARDUINO
    #include <RTClib.h>
    #define PRINT(str) Serial.print(str)
    #define PRINTLN(str) Serial.println(str);
    #define NUMBER_TO_STR(value) String(value)
#else
    #include <iostream>
    #include <sstream>
    #include <string>
    
    #include "DateTime.h"
    #include "TimeSpan.h"
    
    #define String std::string
    #define PRINT(str) std::cout << str;
    #define PRINTLN(str) std::cout << str << std::endl;
    #define NUMBER_TO_STR(value) std::to_string(value)
#endif

String date2string(DateTime dt);
String date2stringShort(DateTime dt);

#endif