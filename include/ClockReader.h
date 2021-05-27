#ifdef ARDUINO

#ifndef CLOCK_READER_H
#define CLOCK_READER_H

#include "RTClib.h"
#include "ClockReaderBase.h"

class ClockReader : public ClockReaderBase
{
    RTC_DS3231 clock;

    void rtcOn(int inputDelayMilliseconds);
    void rtcOff();

  public:
    DateTime activateRtcClockAndReadTime() override;
};

#endif
#endif