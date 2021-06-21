#ifndef EXECUTION_DECIDER_UTEST_H
#define EXECUTION_DECIDER_UTEST_H

#include "ExecutionDecider.h"
#include "ClockReaderMock.h"
#include "CrossPlatformFunctions.h"

class ExecutionDecider_uTest
{
    static String ProbeExecutionDecider(DateTime startTime, 
                                        ClockReaderMock *clockReaderMock,
                                        TimeKeeper *timeKeeper,
                                        ExecutionDecider executionDecider,
                                        int hourOfExecution,
                                        int minuteOfExecution,
                                        int numberOfDaysToCheck,
                                        double timerInterval);

  public:
    static void test_Execution_Decider_Basically_Requests_Execution_Correctly(void);
    static void test_Execution_Next_Day(void);
    static void test_Execution_Almost_Full_Day_Ahead(void);
    static void test_Starting_Exactly_At_Execution_Time(void);
    static void test_With_Deviation_Of_Timer(void);
    static void test_Many_Days_With_Deviation(void);
};

#endif