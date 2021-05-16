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

void AssertExecutionDeciderTiggersCorrectly(DateTime startTime, ClockReaderMock* clockReaderMock, ExecutionDecider executionDecider, int hourOfExecution, int minuteOfExecution, int numberOfDaysToCheck)
{
    DateTime currentTime = startTime;
    
    int i = 0; int numberOfDaysChecked = 0;
    while (true)
    {
        clockReaderMock->setCurrentDateTime(currentTime);
        executionDecider.watchdogInterruptHappened(i);
        bool executionRequested = executionDecider.shouldWeExecute();

        bool timeElapsed = currentTime.hour() == hourOfExecution && currentTime.minute() == minuteOfExecution;
        if (timeElapsed)
        {
            if (!executionRequested)
            {
                String message = "We should execute at " + to_string(currentTime);
                TEST_FAIL_MESSAGE(C_STR(message));
            }

            numberOfDaysChecked++;

            if(numberOfDaysChecked == numberOfDaysToCheck)
            {
                break;
            }
        }
        else
        {
            if (executionRequested)
            {
                String message = "We should not execute at " + to_string(currentTime);
                TEST_FAIL_MESSAGE(C_STR(message));
            }
        }

        currentTime = currentTime + TimeSpan(8);
        i++;
    }
}

void test_Execution_Decider_Basically_Request_Execution_Correctly(void)
{
    int exampleHourOfExecution = 5;
    int exampleMinuteOfExecution = 1;

    ClockReaderMock clockReaderMock = ClockReaderMock();
    ExecutionDecider executionDecider = 
        ExecutionDecider(8, 450, exampleHourOfExecution, exampleMinuteOfExecution, &clockReaderMock);

    DateTime currentTime = DateTime(2010, 1, 1, 4, 59, 1);

    AssertExecutionDeciderTiggersCorrectly(currentTime, &clockReaderMock, executionDecider, 
                                           exampleHourOfExecution, exampleMinuteOfExecution, 1);
}

void test_Execution_Next_Day(void)
{
    int exampleHourOfExecution = 1;
    int exampleMinuteOfExecution = 20;

    ClockReaderMock clockReaderMock = ClockReaderMock();
    ExecutionDecider executionDecider = 
        ExecutionDecider(8, 450, exampleHourOfExecution, exampleMinuteOfExecution, &clockReaderMock);

    DateTime currentTime = DateTime(2010, 1, 1, 23, 59, 3);

    AssertExecutionDeciderTiggersCorrectly(currentTime, &clockReaderMock, executionDecider, 
                                          exampleHourOfExecution, exampleMinuteOfExecution, 1);
}

void test_Execution_Almost_Full_Day_Ahead(void)
{
    int exampleHourOfExecution = 1;
    int exampleMinuteOfExecution = 20;

    ClockReaderMock clockReaderMock = ClockReaderMock();
    ExecutionDecider executionDecider = 
        ExecutionDecider(8, 450, exampleHourOfExecution, exampleMinuteOfExecution, &clockReaderMock);

    DateTime currentTime = DateTime(2010, 1, 1, 1, 30, 0);

    AssertExecutionDeciderTiggersCorrectly(currentTime, &clockReaderMock, executionDecider, 
                                           exampleHourOfExecution, exampleMinuteOfExecution, 1);
}

void test_Starting_Exactly_At_Execution_Time(void)
{
    int exampleHourOfExecution = 1;
    int exampleMinuteOfExecution = 20;

    ClockReaderMock clockReaderMock = ClockReaderMock();
    ExecutionDecider executionDecider = 
        ExecutionDecider(8, 450, exampleHourOfExecution, exampleMinuteOfExecution, &clockReaderMock);

    DateTime currentTime = DateTime(2010, 1, 1, 1, 20, 0);

    AssertExecutionDeciderTiggersCorrectly(currentTime, &clockReaderMock, executionDecider, 
                                           exampleHourOfExecution, exampleMinuteOfExecution, 1);
}

void doTestExecution()
{
    UNITY_BEGIN();
    
    RUN_TEST(test_Execution_Decider_Basically_Request_Execution_Correctly);
    RUN_TEST(test_Execution_Next_Day);
    RUN_TEST(test_Execution_Almost_Full_Day_Ahead);
    RUN_TEST(test_Starting_Exactly_At_Execution_Time);

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