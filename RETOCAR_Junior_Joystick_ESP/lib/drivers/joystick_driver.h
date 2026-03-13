#pragma once
#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>

typedef struct
{
    int x1;
    int y1;
    int x2;
    int y2;

    bool btn1;
    bool btn2;

} ControlPacket;

void esp_service_init();

void esp_service_send(ControlPacket *data);

bool esp_service_available();

ControlPacket esp_service_get();