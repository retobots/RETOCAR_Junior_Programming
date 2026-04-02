#ifndef CONTROL_DATA_SERVICE_H
#define CONTROL_DATA_SERVICE_H

#include "joystick_driver.h"
#include "data_structs.h"

class ControlDataService {
private:
    JoyStick_Driver& m_joy1; // 
    JoyStick_Driver& m_joy2;

    int m_centerX;
    int m_centerY;
    int m_centerOmega;

    // tạo Buffer cho các giá trị
    int m_bufX[5] = {0};
    int m_bufY[5] = {0};
    int m_bufOmega[5] = {0};
    int m_idxX = 0;
    int m_idxY = 0;
    int m_idxOmega = 0;

    uint8_t m_currentMode = 0;          // Lưu mode hiện tại của xe 
    bool m_lastButtonState = false;     // Kiểm tra nút có bị đè hay không
    unsigned long m_lastPressTime = 0;

    // Các hàm xử lý nội bộ 
    int CalculateAverage(int newValue, int* buffer, int& index);
    int applyDeadzone(int value, int center);
    float CalculateSpeed(int value, int center, float v_max);
    float applyExpo(float mappedValue, float v_max);
public:
    // Constructor
    ControlDataService(JoyStick_Driver& joy1, JoyStick_Driver& joy2);

    void begin();
    void calibrateCenter();
    
    void updateControlData(ControlPacket* packet); 
};

#endif