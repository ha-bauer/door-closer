#include <unity.h>


// void setUp(void) {
// // set stuff up here
// }

// void tearDown(void) {
// // clean stuff up here
// }

void test_function1(void) {
    TEST_ASSERT_EQUAL(32, 32);
}

void doTestExecution() {
    UNITY_BEGIN();

    RUN_TEST(test_function1);

    UNITY_END();
}

#ifdef ARDUINO

#include <Arduino.h>
void setup() {
    // NOTE!!! Wait for >2 secs
    // if board doesn't support software reset via Serial.DTR/RTS
    delay(2000);

    doTestExecution();
}

void loop() {
    digitalWrite(13, HIGH);
    delay(100);
    digitalWrite(13, LOW);
    delay(500);
}

#else

int main(int argc, char **argv) {
    doTestExecution();
    return 0;
}

#endif