#ifndef JOYSTICK_DRIVER_H
#define JOYSTICK_DRIVER_H

#pragma once
#include <Arduino.h>

#define PIN_X 34
#define PIN_Y 35
#define PIN_BTN 13
#define PIN_OMEGA 39

class JoyStick_Driver
{
private:
    int m_pinX, m_pinY, m_pinOmega, m_pinBtn;

public:
    JoyStick_Driver(int x, int y, int omega, int pinBtn);

    void begin();  // Khởi tạo chân

    int readX();   // Đọc X
    int readY();   // Đọc Y
    int readOmega();  // Đọc Omega (xoay)
    bool isButtonPressed();  // Kiểm tra trạng thái nút nhấn
};

#endif