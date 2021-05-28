#ifndef TIME_KEEPER_UTEST_H
#define TIME_KEEPER_UTEST_H

#include "TimeKeeper.h"
#include "ClockReaderMock.h"

class TimeKeeper_uTest
{
    static void letTimeRun(DateTime startTime, DateTime endTime, ClockReaderMock *clockReaderMock, 
                        TimeKeeper timeKeeper, double timerInterval);

  public:
    static void test_Time_Keeper_Basically_Works(void);
};

#endif