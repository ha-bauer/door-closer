#ifndef ABSTRACT_CLOCK_READER_H
#define ABSTRACT_CLOCK_READER_H

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