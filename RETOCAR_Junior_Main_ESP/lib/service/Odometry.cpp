#include "Odometry.h"
#include <math.h>

Odometry::Odometry(float lx, float ly, float wheelRadius, float ticksPerRev) {
    m_lx = lx;
    m_ly = ly;
    m_wheelRadius = wheelRadius;
    m_ticksPerRev = ticksPerRev;
    reset();
}

void Odometry::update(int32_t ticksFL, int32_t ticksFR, int32_t ticksRL, int32_t ticksRR) {
    // 1. Tính số xung chênh lệch (Delta Ticks) kể từ lần đọc cuối
    int32_t dFL = ticksFL - m_prevTicksFL;
    int32_t dFR = ticksFR - m_prevTicksFR;
    int32_t dRL = ticksRL - m_prevTicksRL;
    int32_t dRR = ticksRR - m_prevTicksRR;

    // Lưu lại cho lần sau
    m_prevTicksFL = ticksFL;
    m_prevTicksFR = ticksFR;
    m_prevTicksRL = ticksRL;
    m_prevTicksRR = ticksRR;

    // 2. Chuyển đổi số xung thành quãng đường di chuyển của từng bánh (mét hoặc cm)
    // Công thức: (delta_ticks / total_ticks) * 2 * PI * radius
    float dist_per_tick = (2.0f * M_PI * m_wheelRadius) / m_ticksPerRev;
    
    float distFL = dFL * dist_per_tick;
    float distFR = dFR * dist_per_tick;
    float distRL = dRL * dist_per_tick;
    float distRR = dRR * dist_per_tick;

    // 3. Động học thuận Mecanum (Forward Kinematics)
    // Tính sự thay đổi vị trí của tâm xe (dx, dy) và thay đổi góc (dTheta)
    float dx = (distFL + distFR + distRL + distRR) / 4.0f;
    float dy = (-distFL + distFR + distRL - distRR) / 4.0f;
    float dTheta = (-distFL + distFR - distRL + distRR) / (4.0f * (m_lx + m_ly));

    // 4. Cập nhật tọa độ toàn cục (Global Frame) dựa trên góc heading hiện tại
    m_posX += dx * cos(m_heading) - dy * sin(m_heading);
    m_posY += dx * sin(m_heading) + dy * cos(m_heading);
    m_heading += dTheta;

    // Giữ góc heading trong khoảng -PI đến PI
    if (m_heading > M_PI) m_heading -= 2.0f * M_PI;
    if (m_heading < -M_PI) m_heading += 2.0f * M_PI;
}

float Odometry::getX() { return m_posX; }
float Odometry::getY() { return m_posY; }
float Odometry::getHeading() { return m_heading; }

void Odometry::reset() {
    m_posX = 0.0f; m_posY = 0.0f; m_heading = 0.0f;
    m_prevTicksFL = 0; m_prevTicksFR = 0; m_prevTicksRL = 0; m_prevTicksRR = 0;
}