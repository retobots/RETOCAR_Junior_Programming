#ifndef IMU_DRIVER_H
#define IMU_DRIVER_H

#include <Arduino.h>
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"

class IMUDriver {
public:
    IMUDriver();
    bool begin();    
    void readPRY(float &pitch, float &roll, float &yaw);

private:
    MPU6050 mpu;

    // DMP status
    bool dmpReady;
    uint16_t packetSize;
    uint8_t fifoBuffer[64];

    // Orientation variables
    Quaternion q;
    VectorFloat gravity;
    float ypr[3];

    // Lưu lại giá trị cũ phòng khi polled data chưa sẵn sàng
    float lastPitch;
    float lastRoll;
    float lastYaw;
};

#endif // IMU_DRIVER_H
