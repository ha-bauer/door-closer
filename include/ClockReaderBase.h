#ifndef CLOCK_READER_BASE_H
#define CLOCK_READER_BASE_H

#include "DateTime.h"

class ClockReaderBase
{
  public:
    virtual DateTime activateRtcClockAndReadTime() = 0;
};

#endif