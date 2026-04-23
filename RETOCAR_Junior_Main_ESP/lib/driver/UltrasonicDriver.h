#ifndef ULTRASONIC_DRIVER_H
#define ULTRASONIC_DRIVER_H

#include <Arduino.h>

class UltrasonicDriver {
public:
    UltrasonicDriver();
    void begin();
    void trigger();
    float getDistanceCm();

    // Must be static to be used as an ISR
    static void IRAM_ATTR echoISR();
};

#endif // ULTRASONIC_DRIVER_H
