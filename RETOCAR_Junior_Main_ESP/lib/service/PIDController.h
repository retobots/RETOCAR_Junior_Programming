#ifndef PID_CONTROLLER_H
#define PID_CONTROLLER_H

class PIDController {
private:
    float m_kp;         // Hệ số Tỉ lệ
    float m_ki;         // Hệ số Tích phân
    float m_kd;         // Hệ số Vi phân
    float m_prevError;  // Sai số lần trước
    float m_integral;   // Tích lũy sai số
    float m_outMin;     // Giới hạn dưới của Output
    float m_outMax;     // Giới hạn trên của Output

public:
    PIDController(float kp, float ki, float kd, float outMin, float outMax);

    // Cập nhật hệ số (Dùng cho Live Tuning)
    void setTunings(float kp, float ki, float kd);

    // Tính toán Output PWM/Tốc độ
    float compute(float setpoint, float measuredValue, float dt);

    // Reset bộ đệm khi xe dừng
    void reset();
};

#endif