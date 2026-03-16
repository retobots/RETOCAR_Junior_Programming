#pragma once
#include <Arduino.h>

#define PIN_X 39
#define PIN_Y 36
#define PIN_BTN 34
#define PIN_OMEGA 35

class JoyStick_Data
{
private:
    int pinX;
    int pinY;
    int pinOmega;
    int pinBtn;

    int centerX;
    int centerY;
    int centerOmega;

    int deadzone;

    int readADC(int pin);
    int applyDeadzone(int value, int center);
    int normalize(int value, int center);
    int expo(int value);

public:
    JoyStick_Data(int x, int y, int omega, int btn, int dz = 100);

    void begin();
    void calibrate();

    int getX();
    int getY();
    int getOmega();

    bool getButton();
};