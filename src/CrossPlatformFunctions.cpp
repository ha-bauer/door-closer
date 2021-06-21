#include "CrossPlatformFunctions.h"

String date2string(DateTime dt)
{
    String result;

    #ifdef ARDUINO
        char format[] = "YYYY-MM-DD hh:mm:ss";
        result = dt.toString(format);

    #else
        std::stringstream resultStream;
        resultStream << unsigned(dt.year()) << "-";
        resultStream << unsigned(dt.month()) << "-";
        resultStream << unsigned(dt.day()) << " ";
        resultStream << unsigned(dt.hour()) << ":";
        resultStream << unsigned(dt.minute()) << ":";
        resultStream << unsigned(dt.second());
        result = resultStream.str();
    #endif

    return result;
}

String date2stringShort(DateTime dt)
{
    String result;

    #ifdef ARDUINO
        char format[] = "YYYY-MM-DD hh:mm";
        result = dt.toString(format);

    #else
        std::stringstream resultStream;
        resultStream << unsigned(dt.year()) << "-";
        resultStream << unsigned(dt.month()) << "-";
        resultStream << unsigned(dt.day()) << " ";
        resultStream << unsigned(dt.hour()) << ":";
        resultStream << unsigned(dt.minute());
        result = resultStream.str();
    #endif

    return result;
}