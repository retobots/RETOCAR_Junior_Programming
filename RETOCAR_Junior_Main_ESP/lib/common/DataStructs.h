#ifndef DATA_STRUCTS_HPP
#define DATA_STRUCTS_HPP
#include <Arduino.h>

// --- GÓI TIN ESP-NOW (Sóng Radio) ---
// Gói tin từ Joystick gửi sang Master
struct ControlPacket {
    int8_t vx;
    int8_t vy;
    int8_t omega;
    uint8_t mode;
    uint8_t buzzer;
} __attribute__((packed));

// Gói tin Master báo cáo về Joystick
struct TelemetryPacket {
    float posX;
    float posY;
    float heading;
    uint16_t battery;
    uint8_t sensors;
    float distance;
} __attribute__((packed));

// --- BIẾN DÙNG CHUNG TRONG MAIN.CPP ---
// Trạng thái sức khỏe/cảm biến của xe
struct RobotState {
    float posX;
    float posY;
    float heading;
    uint8_t lineSensors;
    float distance;
    uint16_t battery;
};

// Lệnh điều khiển xe
struct RobotCommand {
    int8_t vx;
    int8_t vy;
    int8_t omega;
    uint8_t mode;
    uint8_t buzzer;
};

#endif