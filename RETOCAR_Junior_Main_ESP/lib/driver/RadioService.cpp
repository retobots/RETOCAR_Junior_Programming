#include "RadioService.h"

RadioService radioService;

void RadioService::begin() {
    WiFi.mode(WIFI_STA); 
    if (esp_now_init() != ESP_OK) {
        Serial.println("ESP-NOW Init Failed!");
        return;
    }
    esp_now_register_recv_cb(RadioService::onDataRecv);
    Serial.println("Radio Service Started. Đang chờ Joystick kết nối...");
}


void RadioService::onDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {

    // Nếu chưa kết bạn với Joystick nào thì học lỏm MAC của Joystick đầu tiên gửi dữ liệu tới
    if (!radioService._isPaired) {
        memcpy(radioService._joystickMac, mac, 6); // Lưu MAC của Joystick vào biến toàn cục
        
        esp_now_peer_info_t peerInfo = {};
        memcpy(peerInfo.peer_addr, radioService._joystickMac, 6);
        peerInfo.channel = 0;  
        peerInfo.encrypt = false;
        
        if (esp_now_add_peer(&peerInfo) == ESP_OK) {
            radioService._isPaired = true; // Bật cờ đã kết bạn
            //Serial.println("Đã lấy thành công MAC của Joystick!");
        }
    }
    // Kiểm tra nếu dữ liệu nhận được đúng kích thước của ControlPacket thì mới xử lý
    if (len == sizeof(ControlPacket)) {
        ControlPacket* packet = (ControlPacket*)incomingData;
        
        radioService._tempCmd.vx = packet->vx;
        radioService._tempCmd.vy = packet->vy;
        radioService._tempCmd.omega = packet->omega;
        radioService._tempCmd.mode = packet->mode;
        radioService._tempCmd.buzzer = packet->buzzer;  
        
        radioService._isNewData = true; 
    }
}
bool RadioService::receive() {
    return _isNewData;
}

RobotCommand RadioService::getCmd() {
    _isNewData = false; // Tắt cờ
    return _tempCmd;    // Giao lệnh cho main.cpp
}

void RadioService::send(RobotState state) {
    if (!radioService._isPaired) return; 

    TelemetryPacket tele;
    tele.posX = state.posX;
    tele.posY = state.posY;
    tele.heading = state.heading;
    tele.battery = state.battery;
    tele.sensors = state.lineSensors;
    tele.distance = state.distance;
    // Gửi đích danh (Unicast) tới Joystick
    esp_now_send(radioService._joystickMac, (uint8_t *) &tele, sizeof(tele));
}