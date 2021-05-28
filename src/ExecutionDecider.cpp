#include "ExecutionDecider.h"
#include "CrossPlatformFunctions.h"

ExecutionDecider::ExecutionDecider(int hourOfExecution, int minuteOfExecution, TimeKeeper* timeKeeper)
{
    this->hourOfExecution = hourOfExecution;
    this->minuteOfExecution = minuteOfExecution;
    this->shouldExecute = false;
    this->timeKeeper = timeKeeper;
}

DateTime ExecutionDecider::calculateTimeOfNextExecution(uint32_t unixtimeNow)
{
    DateTime now = DateTime(unixtimeNow);
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

void ExecutionDecider::watchdogInterruptHappened(uint32_t watchdogTickCounter)
{
    watchdogTicksCounter = watchdogTickCounter;

    if (watchdogTicksCounter == 0)
    {
        uint32_t now = timeKeeper->getTime();
        timeOfNextExecution = calculateTimeOfNextExecution(now);
    }

    uint32_t timeOfExecution = timeOfNextExecution.unixtime();

    // String msg = 
    //     "calculated: " + date2string(DateTime(calculatedUnixTimeNow)) + " " +
    //     "atExecution: " + date2string(DateTime(unixTimeAtExecution)) + " " +
    //     "deviationFactor: " + date2string(deviationFactor * 1000) + "/1000";
    // PRINTLN(msg);

    uint32_t approximatedTimeNow = timeKeeper->getTime();

    bool isTimeToExecute = (approximatedTimeNow >= timeOfExecution);
    if (isTimeToExecute)
    {
        shouldExecute = true;
        DateTime slightlyInFuture = DateTime(approximatedTimeNow) + TimeSpan(1);
        timeOfNextExecution = calculateTimeOfNextExecution(slightlyInFuture.unixtime());
    }
    else
    {
        shouldExecute = false;
    }
}

bool ExecutionDecider::shouldWeExecute()
{
    return shouldExecute;
}
