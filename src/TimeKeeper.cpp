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
    this->watchdogTickCounterAtSync = 0;
}

double TimeKeeper::calculateDeviationFactor(uint32_t timeAfter, uint32_t timeBefore)
{
    uint32_t actualTimeElapsed = timeAfter - timeBefore;
    uint32_t theoreticalTimeElapsed = (watchdogTicksCurrently - watchdogTickCounterAtSync) * definedSecondsPerSleepCycle;

    double deviationFactor = ((double) actualTimeElapsed) / ((double) (theoreticalTimeElapsed));

    // PRINTLN(NUMBER_TO_STR(actualTimeElapsed));
    // PRINTLN(NUMBER_TO_STR(theoreticalTimeElapsed));
    // PRINTLN(NUMBER_TO_STR(deviationFactor));
    // PRINTLN("-----");

    return deviationFactor;
}

void TimeKeeper::syncWithRtcAndResetCounters()
{
    DateTime now = clockReader->activateRtcClockAndReadTime();
    uint32_t unixtimeNow = now.unixtime();

    if(watchdogTicksCurrently > 1)
    {
        deviationFactor = calculateDeviationFactor(unixtimeNow, unixtimeAtSync);
    }
    else
    {
        deviationFactor = 1;
    }

    unixtimeAtSync = unixtimeNow;
    watchdogTickCounterAtSync = watchdogTicksCurrently;

    // String msg = 
    //     "unixtimeAtSync: " + NUMBER_TO_STR(unixtimeAtSync) + " " + 
    //     "watchdogTickCounterAtSync: " + NUMBER_TO_STR(watchdogTickCounterAtSync);
    // PRINTLN(msg);
}

void TimeKeeper::watchdogInterruptHappened()
{
    watchdogTicksSinceSync = watchdogTicksCurrently - watchdogTickCounterAtSync;

    bool shouldSyncWithRtcClock = (watchdogTicksCurrently % this->rtcSyncIntervalInCycles == 0);
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

    watchdogTicksCurrently++;
}

uint32_t TimeKeeper::getTime()
{
    return currentCalculatedTime;
}

