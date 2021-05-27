#ifndef CLOCK_READER_MOCK_H
#define CLOCK_READER_MOCK_H

#include "RTClib.h"
#include "ClockReaderBase.h"

class ClockReaderMock : public ClockReaderBase
{
    DateTime now;

  public:
    DateTime activateRtcClockAndReadTime();
    void setCurrentDateTime(DateTime now);
};

#endif