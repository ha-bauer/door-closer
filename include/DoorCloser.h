#ifdef ARDUINO

#ifndef DOOR_CLOSER_H
#define DOOR_CLOSER_H

#include <Arduio.h>

struct doorCloserConfig
{
    int stepPin;
    int dirPin;
    int delayHigh;
    int delayLow;
    int stepsPerRevolution;
    double numRotations;
};

class DoorCloser
{
    doorCloserConfig config;

    void doRotation(int direction, double numRotations);

public:
    DoorCloser(struct doorCloserConfig config);
    void closeDoor();
};

#endif
#endif