#include <Arduino.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>
#include "ExecutionDecider.h"

const int stepPin = 10;
const int dirPin = 11;
const int delayHigh = 800;
const int delayLow = 800;
const int stepsPerRevolution = 1600;
const double numRotations = 0.5;

const int hourOfExecution = 4;
const int minuteOfExecution = 30;

static volatile uint32_t watchdogTickCounter = 0;

static ExecutionDecider executionDecider = ExecutionDecider(8, 450, hourOfExecution, minuteOfExecution);

ISR(WDT_vect)
{
    watchdogTickCounter++;
}

void enter_sleep(void)
{                                        /* Arduino schlafen legen */
    set_sleep_mode(SLEEP_MODE_PWR_DOWN); /* Es geht auch SLEEP_MODE_PWR_DOWN */
    sleep_enable();
    power_adc_disable();    /* Analog-Eingaenge abschalten */
    power_spi_disable();    /* SPI abschalten */
    power_timer0_disable(); /* Timer0 abschalten */
    power_timer2_disable(); /* Timer0 abschalten */
    power_twi_disable();    /* TWI abschalten */
    sleep_mode();
    sleep_disable();
    power_all_enable(); /* Komponenten wieder aktivieren */
}

void startWatchdogTimer()
{
    MCUSR &= ~(1 << WDRF);              /* WDT reset flag loeschen */
    WDTCSR |= (1 << WDCE) | (1 << WDE); /* WDCE setzen, Zugriff auf Presclaler etc. */
    WDTCSR = 1 << WDP0 | 1 << WDP3;     /* Prescaler auf 8.0 s */
    WDTCSR |= _BV(WDIE);                /* WDT Interrupt freigeben */

    //WDTCSR  = (0<<WDP3) | (1<<WDP2) | (1<<WDP1) | (1<<WDP0);
    // Enable the WD interrupt (note: no reset).
    //WDTCSR |= _BV(WDIE);
}

void setup()
{
    Serial.begin(9600);
    Serial.println("Setup...");

    startWatchdogTimer();
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

    delay(1000); // One second delay

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

void loop()
{
    //Serial.println("loop");

    executionDecider.watchdogInterruptHappened(watchdogTickCounter);

    if (executionDecider.shouldWeExecute())
    {
        closeDoor();
    }

    // blink(13);
    // rtcOn(1000);
    // delay(2000);

    // blink(13);
    // rtcOff();

    // delay(2000);
    // blink(13);

    enter_sleep();
}
