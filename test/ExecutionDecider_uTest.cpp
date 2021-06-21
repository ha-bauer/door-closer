#include <unity.h>
#include "ClockReaderMock.h"
#include "ExecutionDecider_uTest.h"
#include "CrossPlatformFunctions.h"

String ExecutionDecider_uTest::ProbeExecutionDecider(DateTime startTime, 
                                                     ClockReaderMock *clockReaderMock,
                                                     TimeKeeper *timeKeeper,
                                                     ExecutionDecider executionDecider,
                                                     int hourOfExecution,
                                                     int minuteOfExecution,
                                                     int numberOfDaysToCheck,
                                                     double timerInterval)
{
    String result;

    DateTime currentTime = startTime;
    DateTime endTime = startTime + TimeSpan(numberOfDaysToCheck, 0, 0, 0);

    int i = 0;
    while (currentTime < endTime)
    {
        currentTime = startTime + TimeSpan(timerInterval * i);
        clockReaderMock->setCurrentDateTime(currentTime);
        timeKeeper->watchdogInterruptHappened();
        executionDecider.watchdogInterruptHappened();
        bool executionRequested = executionDecider.shouldWeExecute();

        // PRINTLN("currentTime:" + date2string(currentTime));
        // PRINTLN("clockReaderMock->activateRtcClockAndReadTime():" + date2string(clockReaderMock->activateRtcClockAndReadTime()));
        // PRINTLN("timeKeeper->getTime():" + date2string(timeKeeper->getTime()));

        if (executionRequested)
        {
            if (result.length() > 0)
            {
                result = result + " | ";
            }

            result = result + date2stringShort(currentTime);
        }

        i++;
    }

    return result;
}

void AssertStringsAreEqual(String str1, String str2)
{
    TEST_ASSERT_EQUAL_STRING(str1.c_str(), str2.c_str());
}

void ExecutionDecider_uTest::test_Execution_Decider_Basically_Requests_Execution_Correctly(void)
{
    int exampleHourOfExecution = 5;
    int exampleMinuteOfExecution = 32;

    ClockReaderMock clockReaderMock = ClockReaderMock();
    TimeKeeper timeKeeper = TimeKeeper(8, 450, &clockReaderMock);
    ExecutionDecider executionDecider =
        ExecutionDecider(exampleHourOfExecution, exampleMinuteOfExecution, &timeKeeper);

    DateTime currentTime = DateTime(2010, 1, 1, 4, 51, 1);

    String executionLog = ProbeExecutionDecider(currentTime, &clockReaderMock, &timeKeeper, executionDecider,
                                                exampleHourOfExecution, exampleMinuteOfExecution, 2, 8);

    AssertStringsAreEqual("2010-1-1 5:32 | 2010-1-2 5:32", executionLog);
}

void ExecutionDecider_uTest::test_Execution_Next_Day(void)
{
    int exampleHourOfExecution = 4;
    int exampleMinuteOfExecution = 15;

    ClockReaderMock clockReaderMock = ClockReaderMock();
    TimeKeeper timeKeeper = TimeKeeper(8, 450, &clockReaderMock);
    ExecutionDecider executionDecider =
        ExecutionDecider(exampleHourOfExecution, exampleMinuteOfExecution, &timeKeeper);

    DateTime currentTime = DateTime(2010, 1, 1, 15, 21, 3);

    String executionLog = ProbeExecutionDecider(currentTime, &clockReaderMock, &timeKeeper, executionDecider,
                                                exampleHourOfExecution, exampleMinuteOfExecution, 2, 8);

    AssertStringsAreEqual("2010-1-2 4:15 | 2010-1-3 4:15", executionLog);
}

void ExecutionDecider_uTest::test_Execution_Almost_Full_Day_Ahead(void)
{
    int exampleHourOfExecution = 1;
    int exampleMinuteOfExecution = 20;

    ClockReaderMock clockReaderMock = ClockReaderMock();
    TimeKeeper timeKeeper = TimeKeeper(8, 450, &clockReaderMock);
    ExecutionDecider executionDecider =
        ExecutionDecider(exampleHourOfExecution, exampleMinuteOfExecution, &timeKeeper);

    DateTime currentTime = DateTime(2010, 1, 1, 1, 30, 0);

    String executionLog = ProbeExecutionDecider(currentTime, &clockReaderMock, &timeKeeper, executionDecider,
                                                exampleHourOfExecution, exampleMinuteOfExecution, 2, 8);

    AssertStringsAreEqual("2010-1-2 1:20 | 2010-1-3 1:20", executionLog);
}

void ExecutionDecider_uTest::test_Starting_Exactly_At_Execution_Time(void)
{
    int exampleHourOfExecution = 1;
    int exampleMinuteOfExecution = 20;

    ClockReaderMock clockReaderMock = ClockReaderMock();
    TimeKeeper timeKeeper = TimeKeeper(8, 450, &clockReaderMock);
    ExecutionDecider executionDecider =
        ExecutionDecider(exampleHourOfExecution, exampleMinuteOfExecution, &timeKeeper);

    DateTime currentTime = DateTime(2010, 1, 1, 1, 20, 0);

    String executionLog = ProbeExecutionDecider(currentTime, &clockReaderMock, &timeKeeper, executionDecider,
                                                exampleHourOfExecution, exampleMinuteOfExecution, 1, 8);

    AssertStringsAreEqual("2010-1-1 1:20 | 2010-1-2 1:20", executionLog);
}

void ExecutionDecider_uTest::test_With_Deviation_Of_Timer(void)
{
    int exampleHourOfExecution = 1;
    int exampleMinuteOfExecution = 20;

    ClockReaderMock clockReaderMock = ClockReaderMock();
    TimeKeeper timeKeeper = TimeKeeper(8, 450, &clockReaderMock);
    ExecutionDecider executionDecider =
        ExecutionDecider(exampleHourOfExecution, exampleMinuteOfExecution, &timeKeeper);

    DateTime currentTime = DateTime(2010, 1, 1, 1, 20, 0);

    String executionLog = ProbeExecutionDecider(currentTime, &clockReaderMock, &timeKeeper, executionDecider,
                                                exampleHourOfExecution, exampleMinuteOfExecution, 2, 8.34);

    AssertStringsAreEqual("2010-1-1 1:20 | 2010-1-2 1:20 | 2010-1-3 1:20", executionLog);
}

void ExecutionDecider_uTest::test_Many_Days_With_Deviation(void)
{
    int exampleHourOfExecution = 1;
    int exampleMinuteOfExecution = 20;

    ClockReaderMock clockReaderMock = ClockReaderMock();
    TimeKeeper timeKeeper = TimeKeeper(8, 450, &clockReaderMock);
    ExecutionDecider executionDecider =
        ExecutionDecider(exampleHourOfExecution, exampleMinuteOfExecution, &timeKeeper);

    DateTime currentTime = DateTime(2010, 1, 1, 1, 20, 0);

    String executionLog = ProbeExecutionDecider(currentTime, &clockReaderMock, &timeKeeper, executionDecider,
                                                exampleHourOfExecution, exampleMinuteOfExecution, 14, 8.34);

    String expectedLog = "";
    expectedLog = expectedLog + "2010-1-1 1:20 | 2010-1-2 1:20 | 2010-1-3 1:20 | " +
                                "2010-1-4 1:20 | 2010-1-5 1:20 | 2010-1-6 1:20 | " +
                                "2010-1-7 1:20 | 2010-1-8 1:20 | 2010-1-9 1:20 | " +
                                "2010-1-10 1:20 | 2010-1-11 1:20 | 2010-1-12 1:20 | " + 
                                "2010-1-13 1:20 | 2010-1-14 1:20";

    AssertStringsAreEqual(expectedLog,executionLog);
}