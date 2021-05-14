#include "RTClib.h"

#include "ExecutionDecider.h"
#include "ClockReader.h"
#include "ClockReaderBase.h"

ExecutionDecider::ExecutionDecider(int secondsPerSleepCycle, uint32_t rtcCheckIntervalInCycles, int hourOfExecution, int minuteOfExecution, ClockReaderBase clockReader)
{
    this->secondsPerSleepCycle = secondsPerSleepCycle;
    this->rtcCheckIntervalInCycles = rtcCheckIntervalInCycles;
    this->hourOfExecution = hourOfExecution;
    this->minuteOfExecution = minuteOfExecution;
    this->clockReader = clockReader;
}

void printStatistics(long measuredSeconds, long calculatedSeconds)
{
    Serial.print("measured seconds: ");
    Serial.print(measuredSeconds);
    Serial.print("  calculated seconds: ");
    Serial.print(calculatedSeconds);
    Serial.print("  difference: ");
    Serial.print(calculatedSeconds - measuredSeconds);

    if (measuredSeconds != 0)
    {
        double deviation = double((calculatedSeconds - measuredSeconds) * 100) / measuredSeconds;
        Serial.print("  deviation (%): ");
        Serial.print(String(deviation, 4));
    }

    Serial.println("");
    Serial.flush();
}

DateTime ExecutionDecider::calculateTimeOfNextExecution(DateTime now)
{
    DateTime todaysExecutionTime =
        DateTime(now.year(), now.month(), now.day(), hourOfExecution, minuteOfExecution, 0);

    bool isNextExecutionToday = todaysExecutionTime >= now;
    if (isNextExecutionToday)
    {
        return todaysExecutionTime;
    }
    else
    {
        TimeSpan oneDay = TimeSpan(1, 0, 0, 0);
        DateTime executionTimeTomorrow = todaysExecutionTime + oneDay;
    }
}

void ExecutionDecider::syncWithRtcAndResetCounters()
{
    DateTime now = clockReader.activateRtcClockAndReadTime();
    uint32_t unixtimeNow = now.unixtime();

    uint32_t actualSecondsSinceSync = unixtimeNow - unixtimeAtSync;
    deviationFactor = calculatedSecondsSinceSync / actualSecondsSinceSync;

    unixtimeAtSync = unixtimeNow;
    watchdogTicksSinceSync = 0;
}

void ExecutionDecider::watchdogInterruptHappened(uint32_t watchdogTickCounter)
{
    watchdogTicksSinceSync = watchdogTickCounter - watchdogTickCounterAtSync;

    calculatedSecondsSinceSync =
        watchdogTicksSinceSync * secondsPerSleepCycle * deviationFactor;

    uint32_t unixTimeAtExecution = timeOfNextExecution.unixtime();
    uint32_t calculatedUnixTimeNow = unixtimeAtSync + calculatedSecondsSinceSync;

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

    bool shouldSyncWithRtcClock = ((watchdogTicksSinceSync - 1) % this->rtcCheckIntervalInCycles == 0);
    if (shouldSyncWithRtcClock)
    {
        syncWithRtcAndResetCounters();
    }
}

bool ExecutionDecider::shouldWeExecute()
{
    return shouldExecute;
}
