#include "CrossPlatformFunctions.h"

#ifdef ARDUINO
    #include <RTClib.h>
    #define INT_TO_STR(int) String(int)
    #define PRINT(str) Serial.print(str)
#else
    #include <iostream>
    #include <sstream>
    #include <string>
    #define String std::string
    #include "DateTime.h"
    #include "TimeSpan.h"
    #define INT_TO_STR(value) to_string(value)
    #define PRINT(str) std::cout << str
    using namespace DateTimeUnitTesting;

    std::string to_string(long value)
    {
        std::stringstream resultStream;
        resultStream << unsigned(value);
        return resultStream.str();
    }
#endif


String toString(DateTime dt)
{
    String result;

    #ifdef ARDUINO
        result = dt.toString("YYYY-MM-DD hh:mm:ss");

    #else
        std::stringstream resultStream;
        resultStream << unsigned(dt.year()) << "-";
        resultStream << unsigned(dt.month()) << "-";
        resultStream << unsigned(dt.day()) << " ";
        resultStream << unsigned(dt.hour()) << ":";
        resultStream << unsigned(dt.minute()) << ":";
        resultStream << unsigned(dt.second()) << " ";
        result = resultStream.str();
    #endif

    return result;
}