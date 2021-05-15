#include "DateTime.h"
#include "ClockReaderMock.h"

using namespace DateTimeUnitTesting;

DateTime ClockReaderMock::activateRtcClockAndReadTime()
{
    return now;
}

void ClockReaderMock::setCurrentDateTime(DateTime now)
{
    this-> now = now;
}