#ifndef JOYSTICK_DRIVER_H
#define JOYSTICK_DRIVER_H

#pragma once
#include <Arduino.h>
#include "pinConfig.h"

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