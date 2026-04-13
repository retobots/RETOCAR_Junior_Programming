#ifndef ODOMETRY_H
#define ODOMETRY_H

#include <stdint.h>

class Odometry {
private:
    float m_lx;          // Khoảng cách từ tâm đến trục bánh trước/sau
    float m_ly;          // Khoảng cách từ tâm đến trục bánh trái/phải
    float m_wheelRadius; // Bán kính bánh xe
    float m_ticksPerRev; // Số xung Encoder cho 1 vòng quay bánh xe
    
    // Lưu trữ số xung của lần tính trước
    int32_t m_prevTicksFL, m_prevTicksFR, m_prevTicksRL, m_prevTicksRR;

    // Tọa độ hiện tại của xe
    float m_posX, m_posY, m_heading;

public:
    Odometry(float lx, float ly, float wheelRadius, float ticksPerRev);

    // Hàm cập nhật tọa độ liên tục (gọi mỗi 10ms)
    void update(int32_t ticksFL, int32_t ticksFR, int32_t ticksRL, int32_t ticksRR);

    // Lấy thông tin tọa độ
    float getX();
    float getY();
    float getHeading();

    // Reset tọa độ về 0
    void reset();
};

#endif