#ifdef ARDUINO
    #include <Arduino.h>
#else
    #include "ArduinoFake.h"
#endif

#include <DoorCloser.h>

void DoorCloser::init(struct doorCloserConfig config)
{
    this->config = config;
}

void DoorCloser::closeDoor()
{
    pinMode(config.powerSwitchPin, OUTPUT);
    digitalWrite(config.powerSwitchPin, HIGH);

    pinMode(config.stepPin, OUTPUT);
    pinMode(config.dirPin, OUTPUT);

    doRotation(LOW, config.numRotations);    
    delay(1000);
    doRotation(HIGH, config.numRotations); 

    digitalWrite(config.powerSwitchPin, LOW);
}

void DoorCloser::doRotation(int direction, double numRotations)
{
    digitalWrite(config.dirPin, direction);

    for (int stepCount = 0; stepCount < config.stepsPerRevolution * numRotations; stepCount++)
    {
        digitalWrite(config.stepPin, HIGH);
        delayMicroseconds(config.delayHigh);
        digitalWrite(config.stepPin, LOW);
        delayMicroseconds(config.delayLow);
    }
}