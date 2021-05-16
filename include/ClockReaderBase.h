#ifndef CLOCK_READER_BASE_H
#define CLOCK_READER_BASE_H

#ifdef ARDUINO
    #include <RTClib.h>
#else
    #include "DateTime.h"
    using DateTimeUnitTesting::DateTime;
#endif

class ClockReaderBase
{
  public:
    virtual DateTime activateRtcClockAndReadTime() = 0;
};

#endif