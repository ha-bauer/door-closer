#ifndef EXECUTION_DECIDER_H
#define EXECUTION_DECIDER_H

#include "ClockReader.h"
#include "ClockReaderBase.h"

#ifndef UNIT_TEST
    #include <RTClib.h>
#else
    #include "DateTime.h"
    using namespace DateTimeUnitTesting;
#endif

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

    ClockReaderBase clockReader;

    DateTime calculateTimeOfNextExecution(DateTime now);
    DateTime activateRtcClockAndReadTime();
    void syncWithRtcAndResetCounters();

public:
    ExecutionDecider(int secondsPerSleepCycle, uint32_t rtcCheckIntervalInCycles, int hourOfExecution, int minuteOfExecution, ClockReaderBase clockReader);
    void watchdogInterruptHappened(uint32_t watchdogTickCounter);
    bool shouldWeExecute();
};

#endif