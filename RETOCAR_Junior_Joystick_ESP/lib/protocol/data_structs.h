#pragma once
#include <Arduino.h>

struct ControlPacket
{
    float vx;
    float vy;
    float omega;

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
