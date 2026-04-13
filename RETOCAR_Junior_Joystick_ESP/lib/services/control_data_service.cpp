#include "control_data_service.h"
#include <Arduino.h>

//Set tốc độ tối đa
const float v_max = 10.0f;


ControlDataService::ControlDataService(JoyStick_Driver& joy1, JoyStick_Driver& joy2)
    : m_joy1(joy1), m_joy2(joy2) {}

//Cấu hình button
void ControlDataService::begin() {
    m_joy1.begin(); 
    m_joy2.begin();
}

//Lấy giá trị trung tâm
void ControlDataService::calibrateCenter() {
    delay(200); // Đợi hệ thống ổn định
    m_centerX = m_joy1.readX();
    m_centerY = m_joy1.readY();
    m_centerOmega = m_joy2.readY(); 
}

//Tính giá trị trung bình (lọc nhiễu tín hiệu)
int ControlDataService::CalculateAverage(int newValue, int* buffer, int& index) {
    buffer[index] = newValue;
    index = (index + 1) % 5;
    int sum = 0;
    for (int i = 0; i < 5; i++) {
        sum += buffer[i];
    }
    return sum / 5;
}

//Set Deadzone
int ControlDataService::applyDeadzone(int value, int center) {
    const int DEADZONE_WIDTH = 100;
    if (abs(value - center) < DEADZONE_WIDTH) {
        return center;
    }
    return value;
}

//Tính vận tốc truyền xuống xe
float ControlDataService::CalculateSpeed(int value, int center, float v_max) {
    const float ADC_max = 4095.0f; 
    const float ADC_min = 0.0f;

    if (value == center) return 0.0f;

    float output = 0.0f;
    if (value > center){
        output = (float)(value - center) * v_max / (ADC_max - (float)center);
    } else {
        output = (float)(value - center) * v_max / ((float)center - ADC_min);
    }

    if (output > v_max) return v_max;
    if (output < -v_max) return -v_max;
    return output;
}

//Làm mượt chuyển động quay ( cubic equation )
float ControlDataService::applyExpo(float mappedValue, float v_max) {
    float x = mappedValue / v_max;
    float y = 0.5f * x * x * x + 0.5f * x; // Hàm bậc 3: a*x^3 + (1-a)*x
    return y*v_max;
}



void ControlDataService::updateControlData(ControlPacket* packet) {
    //Đọc giá trị joystick
    int X = m_joy1.readX();
    int Y = m_joy1.readY();
    int Omega = m_joy2.readY(); 

    //Lọc nhiễu 
    int m_X = CalculateAverage(X, m_bufX, m_idxX);
    int m_Y = CalculateAverage(Y, m_bufY, m_idxY);
    int m_Omega = CalculateAverage(Omega, m_bufOmega, m_idxOmega);

    //Xử lý Vùng chết (Deadzone)
    int dzX = applyDeadzone(m_X, m_centerX);
    int dzY = applyDeadzone(m_Y, m_centerY);
    int dzOmega = applyDeadzone(m_Omega, m_centerOmega);

    //Chuẩn hóa về [-100, 100]
    float normX = CalculateSpeed(dzX, m_centerX, v_max);
    float normY = CalculateSpeed(dzY, m_centerY, v_max);
    float normOmega = CalculateSpeed(dzOmega, m_centerOmega, v_max);

    normX = -normX;

    //Áp dụng Expo 
    float finalOmega = applyExpo(normOmega, v_max);

    //Đóng gói dữ liệu
    packet->vx = round(normX * 100.0f) / 100.0f;
    packet->vy = round(normY * 100.0f) / 100.0f;
    packet->omega = round(finalOmega * 100.0f) / 100.0f;
    
    bool isPressed = m_joy1.isButtonPressed();

    if (isPressed && !m_lastButtonState && (millis() - m_lastPressTime > 200)) {
        m_currentMode++;
        if (m_currentMode > 2) m_currentMode = 0; 
        m_lastPressTime = millis();
    }
    m_lastButtonState = isPressed; // Lưu lại trạng thái tay cho vòng lặp sau
    packet->mode = m_currentMode;
}