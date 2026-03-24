#pragma once
#include "joystick_driver.h"
#include "data_structs.h"

class Control_Data_Service {
private:
    JoyStick_Driver joystick1;
    JoyStick_Driver joystick2;

public:
    Control_Data_Service(JoyStick_Driver j1, JoyStick_Driver j2);

    void initialize();  // Khởi tạo các joystick
    void calibrateJoystick();  // Hiệu chuẩn joystick
    ControlPacket getControlData();  // Lấy dữ liệu điều khiển
    int applyDeadzone(int value, int center);  // Áp dụng deadzone
    int normalize(int value, int center);  // Chuẩn hóa giá trị
    int expo(int value);  // Áp dụng expo cho vận tốc xoay
};