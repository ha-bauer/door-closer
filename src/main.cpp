#include <Arduino.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>
#include "ExecutionDecider.h"
#include "ClockReaderBase.h"

const int stepPin = 10;
const int dirPin = 11;
const int delayHigh = 800;
const int delayLow = 800;
const int stepsPerRevolution = 1600;
const double numRotations = 0.5;

const int hourOfExecution = 4;
const int minuteOfExecution = 30;

static volatile uint32_t watchdogTickCounter = 0;
static ClockReaderBase clockReader = ClockReader();
static ExecutionDecider executionDecider = ExecutionDecider(8, 450, hourOfExecution, minuteOfExecution, clockReader);

ISR(WDT_vect)
{
    watchdogTickCounter++;
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

void startWatchdogTimer()
{
    MCUSR &= ~(1 << WDRF);              /* Deletes WDT reset flag */
    WDTCSR |= (1 << WDCE) | (1 << WDE); /* Sets WDCE , access to presclaler etc. */
    WDTCSR = 1 << WDP0 | 1 << WDP3;     /* Prescaler to 8.0s */
    WDTCSR |= _BV(WDIE);                /* Allow WDT interrupt */
}

void blink(int pin)
{
    pinMode(pin, OUTPUT);
    digitalWrite(pin, HIGH);
    delay(100);
    digitalWrite(pin, LOW);
}

void closeDoor()
{
    pinMode(8, OUTPUT);
    digitalWrite(8, HIGH);

    pinMode(stepPin, OUTPUT);
    pinMode(dirPin, OUTPUT);

    digitalWrite(dirPin, LOW);
    for (int x = 0; x < stepsPerRevolution * numRotations; x++)
    {
        digitalWrite(stepPin, HIGH);
        delayMicroseconds(delayHigh);
        digitalWrite(stepPin, LOW);
        delayMicroseconds(delayLow);
    }

    delay(1000);

    digitalWrite(dirPin, HIGH);
    for (int x = 0; x < stepsPerRevolution * numRotations; x++)
    {
        digitalWrite(stepPin, HIGH);
        delayMicroseconds(delayHigh);
        digitalWrite(stepPin, LOW);
        delayMicroseconds(delayLow);
    }

    digitalWrite(8, LOW);
}

#ifndef UNIT_TEST

void setup()
{
    Serial.begin(9600);
    Serial.println("Setup...");

    startWatchdogTimer();
}

void loop()
{
    //Serial.println("loop");

    executionDecider.watchdogInterruptHappened(watchdogTickCounter);

    if (executionDecider.shouldWeExecute())
    {
        closeDoor();
    }

    enter_sleep();
}

#endif