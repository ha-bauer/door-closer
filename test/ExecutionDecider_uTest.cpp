#include <unity.h>
#include "ExecutionDecider.h"
#include "ClockReaderMock.h"

// void setUp(void) {
// // set stuff up here
// }

// void tearDown(void) {
// 
void test_function1(void)
{
    int someHourOfExecution = 5;
    int someMinuteOfExecution = 30;
    ClockReaderBase clockReaderMock = ClockReaderMock();
    ExecutionDecider executionDecider = ExecutionDecider(8, 450, someHourOfExecution, someMinuteOfExecution, clockReaderMock);

    executionDecider.watchdogInterruptHappened(1)
    if (executionDecider.shouldWeExecute())
    {
        TEST_ASSERT_EQUAL(32, 32);
    }

    TEST_ASSERT_EQUAL(32, 32);
}

void doTestExecution()
{
    UNITY_BEGIN();

    RUN_TEST(test_function1);

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