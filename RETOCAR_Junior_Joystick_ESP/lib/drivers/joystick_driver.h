#pragma once
#include <Arduino.h>

#define PIN_X 39
#define PIN_Y 36
#define PIN_BTN 34
#define PIN_OMEGA 35

class JoyStick_Driver
{
private:
    int pinX, pinY, pinOmega;
    bool Mode;

public:
    JoyStick_Data(int x, int y, int omega, bool Mode);

    void initialize();  // Khởi tạo chân

    int ReadX();   // Trục X
    int ReadY();   // Trục Y
    int ReadOmega();  // Trục Omega (xoay)
    bool isButtonPressed();  // Kiểm tra trạng thái nút nhấn
};