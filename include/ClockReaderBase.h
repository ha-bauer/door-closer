#ifndef CLOCK_READER_BASE_H
#define CLOCK_READER_BASE_H

#ifndef UNIT_TEST
    #include <RTClib.h>
#else
    #include "DateTime.h"
    using namespace DateTimeUnitTesting;
#endif

class ClockReaderBase
{
  public:
    virtual DateTime activateRtcClockAndReadTime()
    {
        return DateTime();
    }
};

#endif