#include <unity.h>
#include "ExecutionDecider_uTest.h"
#include "TimeKeeper_uTest.h"

void doTestExecution()
{
    UNITY_BEGIN();

    RUN_TEST(ExecutionDecider_uTest::test_Execution_Decider_Basically_Request_Execution_Correctly);
    RUN_TEST(ExecutionDecider_uTest::test_Execution_Next_Day);
    RUN_TEST(ExecutionDecider_uTest::test_Execution_Almost_Full_Day_Ahead);
    RUN_TEST(ExecutionDecider_uTest::test_Starting_Exactly_At_Execution_Time);
    RUN_TEST(ExecutionDecider_uTest::test_With_Deviation_Of_Timer);
    RUN_TEST(ExecutionDecider_uTest::test_Many_Days_With_Deviation);

    RUN_TEST(TimeKeeper_uTest::test_Time_Keeper_Basically_Works);
    RUN_TEST(TimeKeeper_uTest::test_Time_Keeper_Works_With_Deviation);

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