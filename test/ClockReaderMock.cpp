#include "ClockReaderMock.h"

#ifdef ARDUINO
    #include <RTClib.h>
#else
    #include "DateTime.h"
    using DateTimeUnitTesting::DateTime;
#endif

DateTime ClockReaderMock::activateRtcClockAndReadTime()
{
    return now;
}

void ClockReaderMock::setCurrentDateTime(DateTime now)
{
    this-> now = now;
}