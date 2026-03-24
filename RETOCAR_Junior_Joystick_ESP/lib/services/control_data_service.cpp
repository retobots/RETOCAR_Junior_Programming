#include "control_data_service.h"

//Khởi tạo các đối tượng 
Control_Data_Service::Control_Data_Service(JoyStick_Driver j1, JoyStick_Driver j2)
    : joystick1(j1), joystick2(j2) {}

//Khởi tạo chân button
void Control_Data_Service::initialize() {
    joystick1.initialize();
}

//Lấy dữ liệu tĩnh của joystick để hiệu chuẩn
void Control_Data_Service::calibrateJoystick() {
    vTaskDelay(pdMS_TO_TICKS(200));  
    centerX = joystick1.readX();
    centerY = joystick1.readY();
    centerOmega = joystick2.readY();
}

//đặt vùng chết cho joystick
int Control_Data_Service::applyDeadzone(int value, int center) {
    int deadzone = 100;
    if (abs(value - center) < deadzone)
        return center;
    return value;
}

//chuẩn hóa giá trị về [-100, 100]
int Control_Data_Service::normalize(int value, int center) {
    int range = 2047;
    int output = (value - center) * 100 / range;
    if (output > 100) output = 100;
    if (output < -100) output = -100;
    return output;
}

//làm mượt chuyển động quay cho omega (cubic exponentiation X^3)
int Control_Data_Service::expo(int value) {
    float x = value / 100.0f; // Chuyển giá trị thành dạng float và chuẩn hóa lại với 100
    float y = x * x * x;  // Áp dụng phép mũ bậc 3 để làm mượt chuyển động
    return (int)(y * 100); // Trả về giá trị expo và chuyển về kiểu int
}


ControlPacket Control_Data_Service::getControlData() {
    ControlPacket packet;

    // Lấy giá trị từ các trục của joystick1 và joystick2
    int vx = joystick1.readX();  // Trục X của joystick1
    int vy = joystick1.readY();  // Trục Y của joystick1
    int omega = joystick2.readY();  // Trục Y của joystick2 (omega)

    // Áp dụng deadzone, normalize và expo 
    packet.vx = normalize(applyDeadzone(vx, 100), 0);
    packet.vy = normalize(applyDeadzone(vy, 100), 0);
    packet.omega = expo(normalize(applyDeadzone(omega, 100), 0));

    // Nút nhấn của joystick1
    packet.mode = joystick1.isButtonPressed();  

    return packet;
}