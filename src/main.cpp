#ifndef UNIT_TEST

#include <Arduino.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>
#include "ExecutionDecider.h"
#include "ClockReaderBase.h"
#include "DoorCloser.h"

const int motorPowerSwitchPin = 8;
const int stepPin = 10;
const int dirPin = 11;
const int delayHigh = 800;
const int delayLow = 800;
const int stepsPerRevolution = 1600;
const double numRotations = 0.5;

const int hourOfExecution = 0;
const int minuteOfExecution = 4;

static volatile uint32_t watchdogTickCounter = 0;
static ClockReader clockReader;

static ExecutionDecider executionDecider = 
    ExecutionDecider(8, 450, hourOfExecution, minuteOfExecution, &clockReader);

static DoorCloser doorCloser = DoorCloser();


void enter_sleep(void);
void startWatchdogTimer();

void setup()
{
    Serial.begin(9600);
    Serial.println("Setup...");
    startWatchdogTimer();

    static struct doorCloserConfig closerConfig;
    closerConfig.powerSwitchPin = motorPowerSwitchPin;
    closerConfig.stepPin = stepPin;
    closerConfig.dirPin = dirPin;
    closerConfig.delayHigh = delayHigh;
    closerConfig.delayLow = delayLow;
    closerConfig.stepsPerRevolution = stepsPerRevolution;
    closerConfig.numRotations = numRotations;

    doorCloser.init(closerConfig);
    clockReader = ClockReader();
}

void loop()
{
    doorCloser.closeDoor();
    
    executionDecider.watchdogInterruptHappened(watchdogTickCounter);

    if (executionDecider.shouldWeExecute())
    {
        doorCloser.closeDoor();
    }

    enter_sleep();
}

ISR(WDT_vect)
{
    watchdogTickCounter++;
}

void startWatchdogTimer()
{
    MCUSR &= ~(1 << WDRF);              /* Deletes WDT reset flag */
    WDTCSR |= (1 << WDCE) | (1 << WDE); /* Sets WDCE , access to presclaler etc. */
    WDTCSR = 1 << WDP0 | 1 << WDP3;     /* Prescaler to 8.0s */
    WDTCSR |= _BV(WDIE);                /* Allow WDT interrupt */
}

void enter_sleep(void)
{
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();
    power_adc_disable();
    power_spi_disable();
    power_timer0_disable();
    power_timer2_disable();
    power_twi_disable();
    sleep_mode();
    sleep_disable();
    power_all_enable();
}

#endif