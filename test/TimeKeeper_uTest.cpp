#include <unity.h>
#include "TimeKeeper_uTest.h"

#include "CrossPlatformFunctions.h"

void TimeKeeper_uTest::letTimeRun(DateTime startTime, DateTime endTime, ClockReaderMock *clockReaderMock, 
                        TimeKeeper *timeKeeper, double timerInterval)
{
    DateTime currentTime = startTime;

    int i = 0;
    while (currentTime < endTime)
    {
        currentTime = startTime + TimeSpan(timerInterval * i);
        clockReaderMock->setCurrentDateTime(currentTime);
        timeKeeper->watchdogInterruptHappened();

        i++;
    }
}

void TimeKeeper_uTest::test_Time_Keeper_Basically_Works(void)
{
    ClockReaderMock clockReaderMock = ClockReaderMock();
    TimeKeeper timeKeeper =
        TimeKeeper(8, 450, &clockReaderMock);

    DateTime startTime = DateTime(2010, 1, 1, 4, 59, 1);
    TimeSpan someTimeSpan = TimeSpan(0, 0, 2, 0);
    DateTime endTime = startTime + someTimeSpan;

    letTimeRun(startTime, endTime, &clockReaderMock, &timeKeeper, 8.0);
    // PRINTLN(NUMBER_TO_STR(endTime.unixtime()));
    // PRINTLN(NUMBER_TO_STR(timeKeeper.getTime()));
    TEST_ASSERT_UINT32_WITHIN(10, endTime.unixtime(), timeKeeper.getTime());
}

void TimeKeeper_uTest::test_Time_Keeper_Works_With_Deviation(void)
{
    ClockReaderMock clockReaderMock = ClockReaderMock();
    TimeKeeper timeKeeper =
        TimeKeeper(8, 450, &clockReaderMock);

    DateTime startTime = DateTime(2010, 1, 1, 4, 59, 1);
    TimeSpan someTimeSpan = TimeSpan(1, 4, 5, 0);
    DateTime endTime = startTime + someTimeSpan;

    letTimeRun(startTime, endTime, &clockReaderMock, &timeKeeper, 8.5);
    // PRINTLN(NUMBER_TO_STR(endTime.unixtime()));
    // PRINTLN(NUMBER_TO_STR(timeKeeper.getTime()));
    TEST_ASSERT_UINT32_WITHIN(200, endTime.unixtime(), timeKeeper.getTime());
}