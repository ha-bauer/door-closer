#include <unity.h>
#include "ExecutionDecider.h"
#include "ClockReaderMock.h"

#ifdef ARDUINO
    #include <Arduino.h>
    #define C_STR(str) str.c_str()
#else
    #include "DateTime.h"
    #include <sstream>
    #include <string>
    #define String std::string
    #define C_STR(str) str.substr(0, str.size()).c_str()
    using DateTimeUnitTesting::DateTime;
    using DateTimeUnitTesting::TimeSpan;
#endif

String toString(DateTime dt)
{
    String result;

    #ifdef ARDUINO
        //result = dt.toString("YYYY-MM-DD hh:mm:ss");
        result = result + dt.year() + "-";
        result = result + dt.month() + "-";
        result = result + dt.day() + " ";
        result = result + dt.hour() + ":";
        result = result + dt.minute() + ":";
        result = result + dt.second();

    #else
        std::stringstream resultStream;
        resultStream << unsigned(dt.year()) << "-";
        resultStream << unsigned(dt.month()) << "-";
        resultStream << unsigned(dt.day()) << " ";
        resultStream << unsigned(dt.hour()) << ":";
        resultStream << unsigned(dt.minute()) << ":";
        resultStream << unsigned(dt.second()) << " ";
        result = resultStream.str();
    #endif

    return result;
}

void test_Execution_Decider_Basically_Request_Execution_Correctly(void)
{
    int exampleHourOfExecution = 5;
    int exampleMinuteOfExecution = 1;

    ClockReaderMock clockReaderMock = ClockReaderMock();
    ExecutionDecider executionDecider = ExecutionDecider(8, 450, exampleHourOfExecution, exampleMinuteOfExecution, &clockReaderMock);

    DateTime currentTime = DateTime(2010, 1, 1, 4, 59, 1);
    int i = 0;
    while(true)
    {
        clockReaderMock.setCurrentDateTime(currentTime);
        executionDecider.watchdogInterruptHappened(i);
        bool executionRequested = executionDecider.shouldWeExecute();

        bool timeElapsed = currentTime.hour() == exampleHourOfExecution && currentTime.minute() == exampleMinuteOfExecution;
        if(timeElapsed)
        {
            if(!executionRequested)
            {
                String message = "We should execute at " + toString(currentTime);
                TEST_FAIL_MESSAGE(C_STR(message));
            }
            break;
        }
        else
        {
            if(executionRequested)
            {
                String message = "We should not execute at " + toString(currentTime);
                TEST_FAIL_MESSAGE(C_STR(message));
            }
        }

        currentTime = currentTime + TimeSpan(8);
        i++;
    }
}

void doTestExecution()
{
    UNITY_BEGIN();
    RUN_TEST(test_Execution_Decider_Basically_Request_Execution_Correctly);
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