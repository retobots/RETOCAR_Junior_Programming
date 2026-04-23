#ifndef RADIO_SERVICE_HPP
#define RADIO_SERVICE_HPP
#include <Arduino.h>
#include "DataStructs.h"
#include <WiFi.h>
#include <esp_now.h>

class RadioService {
private:
    RobotCommand _tempCmd;
    // volatile giúp biến cập nhật tức thời khi bị thay đổi trong hàm ngắt/callback
    volatile bool _isNewData = false;
    
    uint8_t _joystickMac[6];    // Mảng 6 byte để lưu MAC của tay cầm
    bool _isPaired = false;     // Đèn báo hiệu đã kết bạn thành công chưa

public:
    void begin();
    bool receive();
    RobotCommand getCmd();
    void send(RobotState state);

    // Callback nhận dữ liệu ESP-NOW
    static void onDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len);
};

extern RadioService radioService; // Để main.cpp sử dụng

#endif