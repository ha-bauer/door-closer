#ifndef CLOCK_READER_MOCK_H
#define CLOCK_READER_MOCK_H

#include "DateTime.h"
#include "ClockReaderBase.h"

using namespace DateTimeUnitTesting;

class ClockReaderMock : public ClockReaderBase
{
    DateTime now;

  public:
    DateTime activateRtcClockAndReadTime();
    void setCurrentDateTime(DateTime now);
};

#endif