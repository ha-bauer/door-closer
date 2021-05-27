#ifdef ARDUINO

#include "ClockReader.h"

void ClockReader::rtcOn(int inputDelayMilliseconds)
{
    pinMode(9, OUTPUT);
    pinMode(12, OUTPUT);
    digitalWrite(9, HIGH);
    delay(inputDelayMilliseconds);
    digitalWrite(12, HIGH);
}

void ClockReader::rtcOff()
{
    digitalWrite(9, LOW);
    digitalWrite(12, LOW);
    pinMode(9, INPUT);
    pinMode(12, INPUT);
}

DateTime ClockReader::activateRtcClockAndReadTime()
{
    rtcOn(1000);
    RTC_DS3231 clock;
    clock.begin();
    delay(300);
    DateTime now = clock.now();
    rtcOff();

    return now;
}

#endif