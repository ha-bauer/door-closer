#include "RTClib.h"
#include "ClockReaderMock.h"

DateTime ClockReaderMock::activateRtcClockAndReadTime()
{
    return now;
}

void ClockReaderMock::setCurrentDateTime(DateTime now)
{
    this-> now = now;
}