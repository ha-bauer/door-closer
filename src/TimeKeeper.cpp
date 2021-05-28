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
}

void TimeKeeper::syncWithRtcAndResetCounters()
{
    DateTime now = clockReader->activateRtcClockAndReadTime();
    uint32_t unixtimeNow = now.unixtime();
    uint32_t actualSecondsSinceSync = unixtimeNow - unixtimeAtSync;

    deviationFactor = ((double) actualSecondsSinceSync) / ((double) calculatedSecondsSinceSync);
    unixtimeAtSync = unixtimeNow;
    watchdogTickCounterAtSync = watchdogTicksCurrently;
}

void TimeKeeper::watchdogInterruptHappened()
{
    watchdogTicksCurrently++;

    if (watchdogTicksCurrently == 0)
    {
        DateTime now = clockReader->activateRtcClockAndReadTime();
        uint32_t unixtimeNow = now.unixtime();

        unixtimeAtSync = unixtimeNow;
        watchdogTickCounterAtSync = watchdogTicksCurrently;
    }

    watchdogTicksSinceSync = watchdogTicksCurrently - watchdogTickCounterAtSync;

    calculatedSecondsSinceSync =
        watchdogTicksSinceSync * definedSecondsPerSleepCycle * deviationFactor;

    currentCalculatedTime = unixtimeAtSync + calculatedSecondsSinceSync;

    // String msg = 
    //     "atSync: " + date2string(unixtimeAtSync) + " (" + date2string(unixtimeAtSync) + ") " +
    //     "calculated: " + date2string(DateTime(calculatedUnixTimeNow)) + " " +
    //     "atExecution: " + date2string(DateTime(unixTimeAtExecution)) + " " +
    //     "deviationFactor: " + date2string(deviationFactor * 1000) + "/1000";
    // PRINTLN(msg);

    bool shouldSyncWithRtcClock = (watchdogTicksSinceSync > 0 && watchdogTicksSinceSync % this->rtcSyncIntervalInCycles == 0);
    if (shouldSyncWithRtcClock)
    {
        // PRINTLN("syncing");
        syncWithRtcAndResetCounters();
    }
}

uint32_t TimeKeeper::getTime()
{
    return currentCalculatedTime;
}

