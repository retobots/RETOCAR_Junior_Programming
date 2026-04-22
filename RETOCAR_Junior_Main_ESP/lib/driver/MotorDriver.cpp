#include "MotorDriver.h"
#include "PinConfig.h"

// Tần số PWM tiêu chuẩn cho IC DRV8833 chống hú
const int PWM_FREQ = 5000;
const int PWM_RES = 8; // Vi điều khiển băm 8-bit, trải dài từ 0 đến 255

// Đăng ký 8 kênh ESP32
const uint8_t CH_FL_IN1 = 0;
const uint8_t CH_FL_IN2 = 1;
const uint8_t CH_FR_IN1 = 2;
const uint8_t CH_FR_IN2 = 3;
const uint8_t CH_RL_IN1 = 4;
const uint8_t CH_RL_IN2 = 5;
const uint8_t CH_RR_IN1 = 6;
const uint8_t CH_RR_IN2 = 7;

MotorDriver motorDriver;

MotorDriver::MotorDriver() {}

void MotorDriver::begin() {
    // 1. Cấu hình độ phân giải cho 8 kênh PWM
    for(int i = 0; i < 8; i++) {
        ledcSetup(i, PWM_FREQ, PWM_RES);
    }

    // 2. Map (Gắn) các kênh PWM vào các Pin vật lý trên Board ESP32
    // Front Left
    ledcAttachPin(PIN_MOTOR_FL_IN1, CH_FL_IN1);
    ledcAttachPin(PIN_MOTOR_FL_IN2, CH_FL_IN2);
    
    // Front Right
    ledcAttachPin(PIN_MOTOR_FR_IN1, CH_FR_IN1);
    ledcAttachPin(PIN_MOTOR_FR_IN2, CH_FR_IN2);
}

void MotorDriver::setSingleMotor(int speed, uint8_t chIN1, uint8_t chIN2) {
    // Siết trần tốc độ, tránh gây nhiễu bit
    speed = constrain(speed, -255, 255);
    
    // Thuật toán Fast Decay (Thả rông 1 đầu, bơm xung đầu còn lại)
    // - Chạy tới
    if (speed > 0) {
        ledcWrite(chIN1, speed);
        ledcWrite(chIN2, 0);
    }
    // - Chạy lùi
    else if (speed < 0) {
        ledcWrite(chIN1, 0);
        ledcWrite(chIN2, abs(speed));
    }
    // - Dừng tự do Coast
    else {
        ledcWrite(chIN1, 0);
        ledcWrite(chIN2, 0);
    }
}

void MotorDriver::setMotorSpeed(int speedFL, int speedFR, int speedRL, int speedRR) {
    setSingleMotor(speedFL, CH_FL_IN1, CH_FL_IN2);
    setSingleMotor(speedFR, CH_FR_IN1, CH_FR_IN2);
    setSingleMotor(speedRL, CH_RL_IN1, CH_RL_IN2);
    setSingleMotor(speedRR, CH_RR_IN1, CH_RR_IN2);
}

void MotorDriver::stopAll() {
    setMotorSpeed(0, 0, 0, 0);
}