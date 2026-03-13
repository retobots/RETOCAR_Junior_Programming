#pragma once
#include <Arduino.h>
#include "protocol/data_structs.h"

#define PIN_vx 36
#define PIN_vy 39
#define PIN_omega 34
#define BTN_1 14

void joystick_init();
void joystick_read(ControlPacket *Data);

void joystick_calibrate();

int readADC(int pin);
int applyDeadzone(int value, int center, int dz);
int normalizeAxis(int value);
float applyExpo(float x, float expo);