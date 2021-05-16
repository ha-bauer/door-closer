#ifndef CLOCK_READER_MOCK_H
#define CLOCK_READER_MOCK_H

#include "ClockReaderBase.h"

#ifdef ARDUINO
    #include <RTClib.h>
#else
    #include "DateTime.h"
    using DateTimeUnitTesting::DateTime;
#endif

class ClockReaderMock : public ClockReaderBase
{
    DateTime now;

  public:
    DateTime activateRtcClockAndReadTime() override;
    void setCurrentDateTime(DateTime now);
};

#endif