#include "ExecutionDecider.h"
#include "ClockReaderBase.h"
#include "CrossPlatformFunctions.h"

#ifdef ARDUINO
    #include <RTClib.h>
#else
    #include <iostream>
    #include <sstream>
    #include <string>
    
    #include "DateTime.h"
    #include "TimeSpan.h"
    using DateTimeUnitTesting::DateTime;
    using DateTimeUnitTesting::TimeSpan;
#endif

ExecutionDecider::ExecutionDecider(int secondsPerSleepCycle, uint32_t rtcSyncIntervalInCycles, int hourOfExecution, int minuteOfExecution, ClockReaderBase* clockReader)
{
    this->secondsPerSleepCycle = secondsPerSleepCycle;
    this->rtcSyncIntervalInCycles = rtcSyncIntervalInCycles;
    this->hourOfExecution = hourOfExecution;
    this->minuteOfExecution = minuteOfExecution;
    this->clockReader = clockReader;
    this->deviationFactor = 1.0;
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
    deviationFactor = calculatedSecondsSinceSync / actualSecondsSinceSync;

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
    //     "unixTimeAtSync: " + to_string(unixtimeAtSync) + " " +
    //     to_string(calculatedUnixTimeNow) + " " + to_string(DateTime(calculatedUnixTimeNow)) + " " +
    //     to_string(unixTimeAtExecution) + " " + to_string(DateTime(unixTimeAtExecution)) + " " +
    //     "deviationFactor: " + to_string(deviationFactor) + " " +
    //     "\n";
    // PRINT(msg);

    bool isTimeToExecute = (calculatedUnixTimeNow >= unixTimeAtExecution);
    if (isTimeToExecute)
    {
        shouldExecute = true;
        DateTime now = DateTime(calculatedUnixTimeNow);
        timeOfNextExecution = calculateTimeOfNextExecution(now);
    }
    else
    {
        shouldExecute = false;
    }

    bool shouldSyncWithRtcClock = (watchdogTicksSinceSync > 0 && watchdogTicksSinceSync % this->rtcSyncIntervalInCycles == 0);
    if (shouldSyncWithRtcClock)
    {
        // PRINT("syncing");
        syncWithRtcAndResetCounters();
    }
}

bool ExecutionDecider::shouldWeExecute()
{
    return shouldExecute;
}
