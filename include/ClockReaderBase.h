#ifndef CLOCK_READER_BASE_H
#define CLOCK_READER_BASE_H

#include "RTClib.h"

class ClockReaderBase
{
  public:
    virtual DateTime activateRtcClockAndReadTime()
    {
        return nullptr;
    }
};

#endif