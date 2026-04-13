#ifndef KINEMATICS_H
#define KINEMATICS_H
    
class Kinematics {
private:
    float m_lx;          // Khoảng cách từ tâm đến trục bánh trước/sau
    float m_ly;          // Khoảng cách từ tâm đến trục bánh trái/phải
    float m_wheelRadius; // Bán kính bánh xe

public:
    Kinematics(float lx, float ly, float wheelRadius);

    void calculateWheelSpeeds(float vx, float vy, float omega, 
                              float &vFL, float &vFR, float &vRL, float &vRR);
};

#endif