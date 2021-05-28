#ifndef EXECUTION_DECIDER_H
#define EXECUTION_DECIDER_H

#include "TimeKeeper.h"

class ExecutionDecider
{
    int hourOfExecution;
    int minuteOfExecution;

    uint32_t watchdogTicksCounter;
    DateTime timeOfNextExecution;
    bool shouldExecute;

    TimeKeeper* timeKeeper;

    DateTime calculateTimeOfNextExecution(uint32_t unixtimeNow);

public:
    ExecutionDecider(int hourOfExecution, int minuteOfExecution, TimeKeeper* timeKeeper);
    void watchdogInterruptHappened(uint32_t watchdogTickCounter);
    bool shouldWeExecute();
};

#endif