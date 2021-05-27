#include "ExecutionDecider.h"
#include "ClockReaderBase.h"
#include "CrossPlatformFunctions.h"

ExecutionDecider::ExecutionDecider(int secondsPerSleepCycle, uint32_t rtcSyncIntervalInCycles, int hourOfExecution, int minuteOfExecution, ClockReaderBase* clockReader)
{
    this->secondsPerSleepCycle = secondsPerSleepCycle;
    this->rtcSyncIntervalInCycles = rtcSyncIntervalInCycles;
    this->hourOfExecution = hourOfExecution;
    this->minuteOfExecution = minuteOfExecution;
    this->clockReader = clockReader;
    this->deviationFactor = 1.0;
    this->shouldExecute = false;
}

DateTime ExecutionDecider::calculateTimeOfNextExecution(DateTime now)
{
    DateTime todaysExecutionTime = DateTime(now.year(), now.month(), now.day(), hourOfExecution, minuteOfExecution, 0);

    bool isNextExecutionToday = todaysExecutionTime >= now;
    if (isNextExecutionToday)
    {
        return todaysExecutionTime;
    }
    else
    {
        TimeSpan oneDay = TimeSpan(1, 0, 0, 0);
        DateTime executionTimeTomorrow = todaysExecutionTime + oneDay;
        return executionTimeTomorrow;
    }
}

void ExecutionDecider::syncWithRtcAndResetCounters()
{
    DateTime now = clockReader->activateRtcClockAndReadTime();
    uint32_t unixtimeNow = now.unixtime();
    uint32_t actualSecondsSinceSync = unixtimeNow - unixtimeAtSync;

    deviationFactor = ((double) actualSecondsSinceSync) / ((double) calculatedSecondsSinceSync);
    unixtimeAtSync = unixtimeNow;
    watchdogTickCounterAtSync = watchdogTicksCurrently;
}

void ExecutionDecider::watchdogInterruptHappened(uint32_t watchdogTickCounter)
{
    watchdogTicksCurrently = watchdogTickCounter;

    if (watchdogTicksCurrently == 0)
    {
        DateTime now = clockReader->activateRtcClockAndReadTime();
        uint32_t unixtimeNow = now.unixtime();

        timeOfNextExecution = calculateTimeOfNextExecution(now);
        unixtimeAtSync = unixtimeNow;
        watchdogTickCounterAtSync = watchdogTicksCurrently;
    }

    watchdogTicksSinceSync = watchdogTicksCurrently - watchdogTickCounterAtSync;

    calculatedSecondsSinceSync =
        watchdogTicksSinceSync * secondsPerSleepCycle * deviationFactor;

    uint32_t unixTimeAtExecution = timeOfNextExecution.unixtime();
    uint32_t calculatedUnixTimeNow = unixtimeAtSync + calculatedSecondsSinceSync;

    // String msg = 
    //     "atSync: " + date2string(unixtimeAtSync) + " (" + date2string(unixtimeAtSync) + ") " +
    //     "calculated: " + date2string(DateTime(calculatedUnixTimeNow)) + " " +
    //     "atExecution: " + date2string(DateTime(unixTimeAtExecution)) + " " +
    //     "deviationFactor: " + date2string(deviationFactor * 1000) + "/1000";
    // PRINTLN(msg);

    bool isTimeToExecute = (calculatedUnixTimeNow >= unixTimeAtExecution);
    if (isTimeToExecute)
    {
        shouldExecute = true;
        DateTime slightlyInFuture = DateTime(calculatedUnixTimeNow) + TimeSpan(1);
        timeOfNextExecution = calculateTimeOfNextExecution(slightlyInFuture);
    }
    else
    {
        shouldExecute = false;
    }

    bool shouldSyncWithRtcClock = (watchdogTicksSinceSync > 0 && watchdogTicksSinceSync % this->rtcSyncIntervalInCycles == 0);
    if (shouldSyncWithRtcClock)
    {
        // PRINTLN("syncing");
        syncWithRtcAndResetCounters();
    }
}

bool ExecutionDecider::shouldWeExecute()
{
    return shouldExecute;
}
