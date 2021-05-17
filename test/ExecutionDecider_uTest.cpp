#include <unity.h>
#include "ExecutionDecider.h"
#include "ClockReaderMock.h"
#include "CrossPlatformFunctions.h"

#ifdef ARDUINO
#include <Arduino.h>
#else
#include <sstream>
#include <string>

#include "DateTime.h"
#include "TimeSpan.h"
using DateTimeUnitTesting::DateTime;
using DateTimeUnitTesting::TimeSpan;
#endif

String ProbeExecutionDecider(DateTime startTime, ClockReaderMock *clockReaderMock, ExecutionDecider executionDecider,
                             int hourOfExecution, int minuteOfExecution, int numberOfDaysToCheck, double timerInterval)
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
                     to_string(currentTime.year()) + "-" +
                     to_string(currentTime.month()) + "-" +
                     to_string(currentTime.day()) + " " +
                     to_string(currentTime.hour()) + ":" +
                     to_string(currentTime.minute());
        }

        i++;
    }

    return result;
}

void AssertStringAreEqual(String str1, String str2)
{
    TEST_ASSERT_EQUAL_STRING(str1.c_str(), str2.c_str());
}

void test_Execution_Decider_Basically_Request_Execution_Correctly(void)
{
    int exampleHourOfExecution = 5;
    int exampleMinuteOfExecution = 32;

    ClockReaderMock clockReaderMock = ClockReaderMock();
    ExecutionDecider executionDecider =
        ExecutionDecider(8, 450, exampleHourOfExecution, exampleMinuteOfExecution, &clockReaderMock);

    DateTime currentTime = DateTime(2010, 1, 1, 4, 59, 1);

    String executionLog = ProbeExecutionDecider(currentTime, &clockReaderMock, executionDecider,
                                                exampleHourOfExecution, exampleMinuteOfExecution, 2, 8);

    AssertStringAreEqual("2010-1-1 5:32 | 2010-1-2 5:32", executionLog);
}

void test_Execution_Next_Day(void)
{
    int exampleHourOfExecution = 1;
    int exampleMinuteOfExecution = 20;

    ClockReaderMock clockReaderMock = ClockReaderMock();
    ExecutionDecider executionDecider =
        ExecutionDecider(8, 450, exampleHourOfExecution, exampleMinuteOfExecution, &clockReaderMock);

    DateTime currentTime = DateTime(2010, 1, 1, 23, 59, 3);

    String executionLog = ProbeExecutionDecider(currentTime, &clockReaderMock, executionDecider,
                                                exampleHourOfExecution, exampleMinuteOfExecution, 2, 8);

    AssertStringAreEqual("2010-1-2 1:20 | 2010-1-3 1:20", executionLog);
}

void test_Execution_Almost_Full_Day_Ahead(void)
{
    int exampleHourOfExecution = 1;
    int exampleMinuteOfExecution = 20;

    ClockReaderMock clockReaderMock = ClockReaderMock();
    ExecutionDecider executionDecider =
        ExecutionDecider(8, 450, exampleHourOfExecution, exampleMinuteOfExecution, &clockReaderMock);

    DateTime currentTime = DateTime(2010, 1, 1, 1, 30, 0);

    String executionLog = ProbeExecutionDecider(currentTime, &clockReaderMock, executionDecider,
                                                exampleHourOfExecution, exampleMinuteOfExecution, 2, 8);

    AssertStringAreEqual("2010-1-2 1:20 | 2010-1-3 1:20", executionLog);
}

void test_Starting_Exactly_At_Execution_Time(void)
{
    int exampleHourOfExecution = 1;
    int exampleMinuteOfExecution = 20;

    ClockReaderMock clockReaderMock = ClockReaderMock();
    ExecutionDecider executionDecider =
        ExecutionDecider(8, 450, exampleHourOfExecution, exampleMinuteOfExecution, &clockReaderMock);

    DateTime currentTime = DateTime(2010, 1, 1, 1, 20, 0);

    String executionLog = ProbeExecutionDecider(currentTime, &clockReaderMock, executionDecider,
                                                exampleHourOfExecution, exampleMinuteOfExecution, 1, 8);

    AssertStringAreEqual("2010-1-1 1:20 | 2010-1-2 1:20", executionLog);
}

void test_With_Deviation_Of_Timer(void)
{
    int exampleHourOfExecution = 1;
    int exampleMinuteOfExecution = 20;

    ClockReaderMock clockReaderMock = ClockReaderMock();
    ExecutionDecider executionDecider =
        ExecutionDecider(8, 450, exampleHourOfExecution, exampleMinuteOfExecution, &clockReaderMock);

    DateTime currentTime = DateTime(2010, 1, 1, 1, 20, 0);

    String executionLog = ProbeExecutionDecider(currentTime, &clockReaderMock, executionDecider,
                                                exampleHourOfExecution, exampleMinuteOfExecution, 2, 8.34);

    AssertStringAreEqual("2010-1-1 1:20 | 2010-1-2 1:20", executionLog);
}

void test_Many_Days_With_Deviation(void)
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

    AssertStringAreEqual(expectedLog,executionLog);
}

void doTestExecution()
{
    UNITY_BEGIN();

    RUN_TEST(test_Execution_Decider_Basically_Request_Execution_Correctly);
    RUN_TEST(test_Execution_Next_Day);
    RUN_TEST(test_Execution_Almost_Full_Day_Ahead);
    RUN_TEST(test_Starting_Exactly_At_Execution_Time);
    RUN_TEST(test_With_Deviation_Of_Timer);
    RUN_TEST(test_Many_Days_With_Deviation);

    UNITY_END();
}

#ifdef ARDUINO

#include <Arduino.h>
void setup()
{
    // NOTE!!! Wait for >2 secs
    // if board doesn't support software reset via Serial.DTR/RTS
    delay(2000);

    doTestExecution();
}

void loop()
{
    digitalWrite(13, HIGH);
    delay(100);
    digitalWrite(13, LOW);
    delay(500);
}

#else

int main(int argc, char **argv)
{
    doTestExecution();
    return 0;
}

#endif