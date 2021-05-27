#ifndef EXECUTION_DECIDER_H
#define EXECUTION_DECIDER_H

#include "ClockReader.h"
#include "ClockReaderBase.h"

class ExecutionDecider
{
    int hourOfExecution;
    int minuteOfExecution;

    int secondsPerSleepCycle;
    int rtcSyncIntervalInCycles;
    uint32_t watchdogTickCounterAtSync;
    uint32_t watchdogTicksSinceSync;
    uint32_t watchdogTicksCurrently;
    uint32_t unixtimeAtSync;
    uint32_t calculatedSecondsSinceSync;

    double deviationFactor;

    DateTime timeOfNextExecution;
    bool shouldExecute;

    ClockReaderBase* clockReader;

    DateTime calculateTimeOfNextExecution(DateTime now);
    DateTime activateRtcClockAndReadTime();
    void syncWithRtcAndResetCounters();

public:
    ExecutionDecider(int secondsPerSleepCycle, uint32_t rtcSyncIntervalInCycles, int hourOfExecution, int minuteOfExecution, ClockReaderBase* clockReader);
    void watchdogInterruptHappened(uint32_t watchdogTickCounter);
    bool shouldWeExecute();
};

#endif