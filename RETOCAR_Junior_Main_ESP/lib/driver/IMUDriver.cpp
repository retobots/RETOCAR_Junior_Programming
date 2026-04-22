#include "IMUDriver.h"
#include "PinConfig.h"
#include <Wire.h>

IMUDriver::IMUDriver() : dmpReady(false), lastPitch(0), lastRoll(0), lastYaw(0) {}

bool IMUDriver::begin() {
    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN, 400000); 
    
    mpu.initialize();
    if (!mpu.testConnection()) {
        return false;
    }
    
    // Khởi tạo DMP
    uint8_t devStatus = mpu.dmpInitialize();
    
    // Bổ sung offsets chuẩn bị cấu hình (có thể tinh chỉnh sau)
    mpu.setXGyroOffset(220);
    mpu.setYGyroOffset(76);
    mpu.setZGyroOffset(-85);
    mpu.setZAccelOffset(1788); 
    
    // devStatus = 0 nghĩa là khởi tạo DMP thành công
    if (devStatus == 0) {
        mpu.CalibrateAccel(6);
        mpu.CalibrateGyro(6);
        
        mpu.setDMPEnabled(true);
        
        dmpReady = true;
        packetSize = mpu.dmpGetFIFOPacketSize();
        return true;
    } else {
        return false;
    }
}

void IMUDriver::readPRY(float &pitch, float &roll, float &yaw) {
    if (!dmpReady) {
        pitch = 0; roll = 0; yaw = 0;
        return;
    }
    
    // Cơ chế POLLING: Đọc ngay gói dữ liệu FIFO hiện tại (sẽ không bị Drop kể cả khi ko dùng ngắt INT)
    if (mpu.dmpGetCurrentFIFOPacket(fifoBuffer)) {
        // Có dữ liệu DMP -> tính Yaw, Pitch, Roll
        mpu.dmpGetQuaternion(&q, fifoBuffer);
        mpu.dmpGetGravity(&gravity, &q);
        mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
        
        // ypr[0] là Yaw, ypr[1] là Pitch, ypr[2] là Roll (đơn vị rad) -> Đổi sang độ 
        // Trừ Yaw bị đổi chiều (tùy thuộc vào phần cứng) chúng ta có thể format chuẩn lại
        lastYaw = ypr[0] * 180.0f / PI; 
        lastPitch = ypr[1] * 180.0f / PI;
        lastRoll = ypr[2] * 180.0f / PI;
    }
    
    // Trả về biến được tham chiếu (reference) bằng trạng thái ổn định nhất
    pitch = lastPitch;
    roll = lastRoll;
    yaw = lastYaw;
}
