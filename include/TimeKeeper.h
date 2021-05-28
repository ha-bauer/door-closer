#ifndef TIME_KEEPER_H
#define TIME_KEEPER_H

#include "ClockReader.h"
#include "ClockReaderBase.h"

class TimeKeeper
{
    uint32_t currentCalculatedTime;
    uint8_t definedSecondsPerSleepCycle;
    uint8_t rtcSyncIntervalInCycles;
    uint32_t watchdogTickCounterAtSync;
    uint32_t watchdogTicksSinceSync;
    uint32_t watchdogTicksCurrently;
    uint32_t unixtimeAtSync;
    uint32_t calculatedSecondsSinceSync;
    
    double deviationFactor;

    ClockReaderBase* clockReader;

    DateTime activateRtcClockAndReadTime();
    void syncWithRtcAndResetCounters();

public:
    TimeKeeper(uint8_t definedSecondsPerSleepCycle, uint32_t rtcSyncIntervalInCycles, ClockReaderBase* clockReader);
    void watchdogInterruptHappened();
    uint32_t getTime();
};

#endif