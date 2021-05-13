#ifndef CLOCK_READER_H
#define CLOCK_READER_H

#include "RTClib.h"

class ClockReader
{
    RTC_DS3231 clock;

    void rtcOn(int inputDelayMilliseconds);
    void rtcOff();

  public:
    DateTime activateRtcClockAndReadTime();
};

#endif