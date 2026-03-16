#include "joystick_driver.h"

JoyStick_Data::JoyStick_Data(int x, int y, int omega, int btn, int dz)
{
    pinX = x;
    pinY = y;
    pinOmega = omega;
    pinBtn = btn;

    deadzone = dz;
}

void JoyStick_Data::begin()
{
    pinMode(pinBtn, INPUT_PULLUP);
}

void JoyStick_Data::calibrate()
{
    delay(200);
    centerX = analogRead(pinX);
    centerY = analogRead(pinY);
    centerOmega = analogRead(pinOmega);
}

int JoyStick_Data::readADC(int pin)
{
    int sum = 0;

    for(int i = 0; i < 5; i++)
    {
        sum += analogRead(pin);
    }
    
    return sum / 5;
}

int JoyStick_Data::applyDeadzone(int value, int center)
{
    if (abs(value - center) < deadzone)
        return center;

    return value;
}

int JoyStick_Data::normalize(int value, int center)
{
    int range = 2047;

    int output = (value - center) * 100 / range;

    if (output > 100) output = 100;
    if (output < -100) output = -100;

    return output;
}

int JoyStick_Data::expo(int value)
{
    float x = value / 100.0f;
    float y = x * x * x;

    return (int)(y * 100);
}

int JoyStick_Data::getX()
{
    int val = readADC(pinX);
    val = applyDeadzone(val, centerX);
    val = normalize(val, centerX);
    val = expo(val);
    return val;
}

int JoyStick_Data::getY()
{
    int val = readADC(pinY);
    val = applyDeadzone(val, centerY);
    val = normalize(val, centerY);
    val = expo(val);
    return val;
}

int JoyStick_Data::getOmega()
{
    int val = readADC(pinOmega);
    val = applyDeadzone(val, centerOmega);
    val = normalize(val, centerOmega);
    val = expo(val);
    return val;
}

bool JoyStick_Data::getButton()
{
    return !digitalRead(pinBtn);
}