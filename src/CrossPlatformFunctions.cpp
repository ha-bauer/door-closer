#include "CrossPlatformFunctions.h"

String date2string(DateTime dt)
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
        resultStream << unsigned(dt.second());
        result = resultStream.str();
    #endif

    return result;
}