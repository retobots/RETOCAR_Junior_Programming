#include "PIDController.h"

PIDController::PIDController(float kp, float ki, float kd, float outMin, float outMax) {
    m_kp = kp; m_ki = ki; m_kd = kd;
    m_outMin = outMin; m_outMax = outMax;
    reset();
}

void PIDController::setTunings(float kp, float ki, float kd) {
    m_kp = kp; m_ki = ki; m_kd = kd;
}

float PIDController::compute(float setpoint, float measuredValue, float dt) {
    float error = setpoint - measuredValue;
    m_integral += error * dt;
    
    // Anti-windup (Chống trôi Tích phân khi xe bị kẹt)
    if (m_integral > m_outMax) m_integral = m_outMax;
    else if (m_integral < m_outMin) m_integral = m_outMin;

    float derivative = (error - m_prevError) / dt;
    float output = (m_kp * error) + (m_ki * m_integral) + (m_kd * derivative);

    // Giới hạn đầu ra
    if (output > m_outMax) output = m_outMax;
    else if (output < m_outMin) output = m_outMin;

    m_prevError = error;
    return output;
}

void PIDController::reset() {
    m_prevError = 0.0f;
    m_integral = 0.0f;
}