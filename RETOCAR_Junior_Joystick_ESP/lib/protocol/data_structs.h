#pragma once
#include <Arduino.h>

struct ControlPacket
{
    int16_t vx;
    int16_t vy;
    int16_t omega;

    uint8_t mode;
    bool buzzer;
};

struct TelemetryPacket{
    float posX;
    float posY;

    float heading;

    float battery;

    uint8_t sensors;

    float distance;
};
