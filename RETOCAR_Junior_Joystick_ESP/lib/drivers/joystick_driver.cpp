#include "joystick_driver.h"

JoyStick_Driver::JoyStick_Driver(int x, int y, int omega, bool mode) {
    pinX = x;
    pinY = y;
    pinOmega = omega;
    Mode = mode;
}

//Cấu hình cho button
void JoyStick_Driver::initialize() {
    pinMode(Mode, INPUT_PULLUP);  // Nút nhấn
}

//Đọc giá trị trục X
int JoyStick_Driver::readX() {
    return analogRead(pinX);  // Đọc giá trị từ trục X
}

//Đọc giá trị Y
int JoyStick_Driver::readY() {
    return analogRead(pinY);  // Đọc giá trị từ trục Y
}

//Đọc giá trị tốc độ quay
int JoyStick_Driver::readOmega() {
    return analogRead(pinOmega);  // Đọc giá trị từ trục Omega (Xoay)
}

//Kiểm tra trạng thái nút nhấn
bool JoyStick_Driver::isButtonPressed() {
    return !digitalRead(Mode);  // Kiểm tra trạng thái nút nhấn
}