#ifndef MOTOR_DRIVER_H
#define MOTOR_DRIVER_H

#include <Arduino.h>

class MotorDriver {
public:
    MotorDriver();
    
    // Khởi tạo các mâm kênh PWM ngoại vi
    void begin();

    // Điều khiển 4 bánh xe (-255 đến 255). Tốc độ > 0 là tới, < 0 là lùi.
    void setMotorSpeed(int speedFL, int speedFR, int speedRL, int speedRR);

    // Kích hoạt dừng hoàn toàn (Coast Mode)
    void stopAll();

private:
    // Cốt lõi xử lý đảo chiều linh hoạt (Fast Decay) cho DRV8833
    void setSingleMotor(int speed, uint8_t channelIN1, uint8_t channelIN2);
};

// Cung cấp đối tượng tham chiếu toàn cục cho main.cpp
extern MotorDriver motorDriver;

#endif // MOTOR_DRIVER_H
