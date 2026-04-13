#include "joystick_driver.h"

JoyStick_Driver::JoyStick_Driver(int x, int y, int omega, int mode) {
    m_pinX = x;
    m_pinY = y;
    m_pinOmega = omega;
    m_pinBtn = mode;
}

//Cấu hình cho button
void JoyStick_Driver::begin() {
    pinMode(m_pinBtn, INPUT_PULLUP); // Nút nhấn
}

// Đọc giá trị trục X
int JoyStick_Driver::readX() {
    return analogRead(m_pinX);  
}

// Đọc giá trị trục Y
int JoyStick_Driver::readY() {
    return analogRead(m_pinY);  
}

//Kiểm tra trạng thái nút nhấn
bool JoyStick_Driver::isButtonPressed() {
    if (PIN_BTN == -1) return false;

    return digitalRead(m_pinBtn) == LOW;
}