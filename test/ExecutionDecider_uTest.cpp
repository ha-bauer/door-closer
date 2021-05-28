#include <unity.h>
#include "ClockReaderMock.h"
#include "ExecutionDecider_uTest.h"

String ExecutionDecider_uTest::ProbeExecutionDecider(DateTime startTime, 
                                                     ClockReaderMock *clockReaderMock,
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
        executionDecider.watchdogInterruptHappened(i);
        bool executionRequested = executionDecider.shouldWeExecute();

        if (executionRequested)
        {
            if (result.length() > 0)
            {
                result = result + " | ";
            }
            result = result +
                     String(currentTime.year()) + "-" +
                     String(currentTime.month()) + "-" +
                     String(currentTime.day()) + " " +
                     String(currentTime.hour()) + ":" +
                     String(currentTime.minute());
        }

        i++;
    }

    return result;
}

void AssertStringsAreEqual(String str1, String str2)
{
    TEST_ASSERT_EQUAL_STRING(str1.c_str(), str2.c_str());
}

void ExecutionDecider_uTest::test_Execution_Decider_Basically_Request_Execution_Correctly(void)
{
    int exampleHourOfExecution = 5;
    int exampleMinuteOfExecution = 32;

    ClockReaderMock clockReaderMock = ClockReaderMock();
    ExecutionDecider executionDecider =
        ExecutionDecider(8, 450, exampleHourOfExecution, exampleMinuteOfExecution, &clockReaderMock);

    DateTime currentTime = DateTime(2010, 1, 1, 4, 59, 1);

    String executionLog = ProbeExecutionDecider(currentTime, &clockReaderMock, executionDecider,
                                                exampleHourOfExecution, exampleMinuteOfExecution, 2, 8);

    AssertStringsAreEqual("2010-1-1 5:32 | 2010-1-2 5:32", executionLog);
}

void ExecutionDecider_uTest::test_Execution_Next_Day(void)
{
    int exampleHourOfExecution = 1;
    int exampleMinuteOfExecution = 20;

    ClockReaderMock clockReaderMock = ClockReaderMock();
    ExecutionDecider executionDecider =
        ExecutionDecider(8, 450, exampleHourOfExecution, exampleMinuteOfExecution, &clockReaderMock);

    DateTime currentTime = DateTime(2010, 1, 1, 23, 59, 3);

    String executionLog = ProbeExecutionDecider(currentTime, &clockReaderMock, executionDecider,
                                                exampleHourOfExecution, exampleMinuteOfExecution, 2, 8);

    AssertStringsAreEqual("2010-1-2 1:20 | 2010-1-3 1:20", executionLog);
}

void ExecutionDecider_uTest::test_Execution_Almost_Full_Day_Ahead(void)
{
    int exampleHourOfExecution = 1;
    int exampleMinuteOfExecution = 20;

    ClockReaderMock clockReaderMock = ClockReaderMock();
    ExecutionDecider executionDecider =
        ExecutionDecider(8, 450, exampleHourOfExecution, exampleMinuteOfExecution, &clockReaderMock);

    DateTime currentTime = DateTime(2010, 1, 1, 1, 30, 0);

    String executionLog = ProbeExecutionDecider(currentTime, &clockReaderMock, executionDecider,
                                                exampleHourOfExecution, exampleMinuteOfExecution, 2, 8);

    AssertStringsAreEqual("2010-1-2 1:20 | 2010-1-3 1:20", executionLog);
}

void ExecutionDecider_uTest::test_Starting_Exactly_At_Execution_Time(void)
{
    int exampleHourOfExecution = 1;
    int exampleMinuteOfExecution = 20;

    ClockReaderMock clockReaderMock = ClockReaderMock();
    ExecutionDecider executionDecider =
        ExecutionDecider(8, 450, exampleHourOfExecution, exampleMinuteOfExecution, &clockReaderMock);

    DateTime currentTime = DateTime(2010, 1, 1, 1, 20, 0);

    String executionLog = ProbeExecutionDecider(currentTime, &clockReaderMock, executionDecider,
                                                exampleHourOfExecution, exampleMinuteOfExecution, 1, 8);

    AssertStringsAreEqual("2010-1-1 1:20 | 2010-1-2 1:20", executionLog);
}

void ExecutionDecider_uTest::test_With_Deviation_Of_Timer(void)
{
    int exampleHourOfExecution = 1;
    int exampleMinuteOfExecution = 20;

    ClockReaderMock clockReaderMock = ClockReaderMock();
    ExecutionDecider executionDecider =
        ExecutionDecider(8, 450, exampleHourOfExecution, exampleMinuteOfExecution, &clockReaderMock);

    DateTime currentTime = DateTime(2010, 1, 1, 1, 20, 0);

    String executionLog = ProbeExecutionDecider(currentTime, &clockReaderMock, executionDecider,
                                                exampleHourOfExecution, exampleMinuteOfExecution, 2, 8.34);

    AssertStringsAreEqual("2010-1-1 1:20 | 2010-1-2 1:20", executionLog);
}

void ExecutionDecider_uTest::test_Many_Days_With_Deviation(void)
{
    int exampleHourOfExecution = 1;
    int exampleMinuteOfExecution = 20;

    ClockReaderMock clockReaderMock = ClockReaderMock();
    ExecutionDecider executionDecider =
        ExecutionDecider(8, 450, exampleHourOfExecution, exampleMinuteOfExecution, &clockReaderMock);

    DateTime currentTime = DateTime(2010, 1, 1, 1, 20, 0);

    String executionLog = ProbeExecutionDecider(currentTime, &clockReaderMock, executionDecider,
                                                exampleHourOfExecution, exampleMinuteOfExecution, 14, 8.34);

    String expectedLog = "";
    expectedLog = expectedLog + "2010-1-1 1:20 | 2010-1-2 1:20 | 2010-1-3 1:20 | " +
                                "2010-1-4 1:20 | 2010-1-5 1:20 | 2010-1-6 1:20 | " +
                                "2010-1-7 1:20 | 2010-1-8 1:20 | 2010-1-9 1:20 | " +
                                "2010-1-10 1:20 | 2010-1-11 1:20 | 2010-1-12 1:20 | " + 
                                "2010-1-13 1:20 | 2010-1-14 1:20";

    AssertStringsAreEqual(expectedLog,executionLog);
}