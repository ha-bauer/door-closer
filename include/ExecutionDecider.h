#ifndef EXECUTION_DECIDER_H
#define EXECUTION_DECIDER_H

#include <Arduino.h>
#include "RTClib.h"

#include "ClockReader.h"

class ExecutionDecider
{
    int hourOfExecution = 0;
    int minuteOfExecution = 0;

    int secondsPerSleepCycle;
    int rtcCheckIntervalInCycles;
    uint32_t watchdogTickCounterAtSync = 0;
    uint32_t watchdogTicksSinceSync;
    uint32_t unixtimeAtSync;
    uint32_t calculatedSecondsSinceSync;

    double deviationFactor = 1;

    DateTime timeOfNextExecution;
    bool shouldExecute = false;

    ClockReader clockReader;

    DateTime calculateTimeOfNextExecution(DateTime now);
    DateTime activateRtcClockAndReadTime();
    void syncWithRtcAndResetCounters();

public:
    ExecutionDecider(int secondsPerSleepCycle, uint32_t rtcCheckIntervalInCycles, int hourOfExecution, int minuteOfExecution, ClockReader clockReader);
    void watchdogInterruptHappened(uint32_t watchdogTickCounter);
    bool shouldWeExecute();
};

#endif