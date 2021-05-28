#include "TimeKeeper.h"
#include "ClockReaderBase.h"
#include "CrossPlatformFunctions.h"

TimeKeeper::TimeKeeper(uint8_t definedSecondsPerSleepCycle, uint32_t rtcSyncIntervalInCycles, 
                       ClockReaderBase* clockReader)
{
    this->definedSecondsPerSleepCycle = definedSecondsPerSleepCycle;
    this->rtcSyncIntervalInCycles = rtcSyncIntervalInCycles;
    this->clockReader = clockReader;
    this->deviationFactor = 1.0;
    this->watchdogTicksCurrently = 0;
    this->watchdogTickCounterAtSync = 1;
}

double TimeKeeper::calculateDeviationFactor(uint32_t timeAfter, uint32_t timeBefore, uint32_t calculatedDelta)
{
    uint32_t actualDelta = timeAfter - timeBefore;
    double deviationFactor = ((double) actualDelta) / ((double) calculatedSecondsSinceSync);
    return deviationFactor;
}

void TimeKeeper::syncWithRtcAndResetCounters()
{
    DateTime now = clockReader->activateRtcClockAndReadTime();
    uint32_t unixtimeNow = now.unixtime();

    if(watchdogTicksCurrently > 1)
    {
        deviationFactor = calculateDeviationFactor(unixtimeNow, unixtimeAtSync, calculatedSecondsSinceSync);
    }
    else
    {
        deviationFactor = 1;
    }

    unixtimeAtSync = unixtimeNow;
    watchdogTickCounterAtSync = watchdogTicksCurrently;
}

void TimeKeeper::watchdogInterruptHappened()
{
    watchdogTicksCurrently++;

    watchdogTicksSinceSync = watchdogTicksCurrently - watchdogTickCounterAtSync;

    bool shouldSyncWithRtcClock = (watchdogTicksSinceSync % this->rtcSyncIntervalInCycles == 0);
    if (shouldSyncWithRtcClock)
    {
        // PRINTLN("syncing");
        syncWithRtcAndResetCounters();
    }

    calculatedSecondsSinceSync =
        watchdogTicksSinceSync * definedSecondsPerSleepCycle * deviationFactor;

    currentCalculatedTime = unixtimeAtSync + calculatedSecondsSinceSync;

    // String msg = 
    //     "atSync: " + NUMBER_TO_STR(unixtimeAtSync) + " (" + date2string(DateTime(unixtimeAtSync)) + ") " +
    //     "calculated: " + NUMBER_TO_STR(currentCalculatedTime) + " (" + date2string(DateTime(currentCalculatedTime)) + ") " +
    //     "deviationFactor: " + NUMBER_TO_STR(deviationFactor);
    // PRINTLN(msg);
}

uint32_t TimeKeeper::getTime()
{
    return currentCalculatedTime;
}

