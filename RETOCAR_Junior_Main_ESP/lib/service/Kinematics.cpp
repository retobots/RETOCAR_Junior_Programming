#include "Kinematics.h"

Kinematics::Kinematics(float lx, float ly, float wheelRadius) {
    m_lx = lx;
    m_ly = ly;
    m_wheelRadius = wheelRadius;
}

void Kinematics::calculateWheelSpeeds(float vx, float vy, float omega, 
                                      float &vFL, float &vFR, float &vRL, float &vRR) {
    float l_sum = m_lx + m_ly; 

    // Động học nghịch Mecanum
    vFL = vx - vy - (omega * l_sum);
    vFR = vx + vy + (omega * l_sum);
    vRL = vx + vy - (omega * l_sum);
    vRR = vx - vy + (omega * l_sum);
}