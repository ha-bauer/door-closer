#ifdef ARDUINO

#include <Arduino.h>
#include <DoorCloser.h>

void DoorCloser::init(struct doorCloserConfig config)
{
    this->config = config;
}

void DoorCloser::closeDoor()
{
    pinMode(config.stepPin, OUTPUT);
    pinMode(config.dirPin, OUTPUT);

    doRotation(LOW, config.numRotations);    
    delay(1000);
    doRotation(HIGH, config.numRotations); 
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

#endif