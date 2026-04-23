#include "Navigator.h"
#include <Arduino.h>
#include "TuningConfig.h"
#include "MotorDriver.h"

Navigator navigator;
// Khởi tạo kích thước xe và các hệ số PID mặc định
Navigator::Navigator() 
    : m_kinematics(ROBOT_LX, ROBOT_LY, ROBOT_WHEEL_RADIUS), 
      m_odometry(ROBOT_LX, ROBOT_LY, ROBOT_WHEEL_RADIUS, ENCODER_TICKS_PER_REV),
      m_pidFL(PID_KP_DEFAULT, PID_KI_DEFAULT, PID_KD_DEFAULT, PID_OUT_MIN, PID_OUT_MAX),
      m_pidFR(PID_KP_DEFAULT, PID_KI_DEFAULT, PID_KD_DEFAULT, PID_OUT_MIN, PID_OUT_MAX),
      m_pidRL(PID_KP_DEFAULT, PID_KI_DEFAULT, PID_KD_DEFAULT, PID_OUT_MIN, PID_OUT_MAX),
      m_pidRR(PID_KP_DEFAULT, PID_KI_DEFAULT, PID_KD_DEFAULT, PID_OUT_MIN, PID_OUT_MAX)
{
}

void Navigator::init() {
    m_odometry.reset();
    m_pidFL.reset(); m_pidFR.reset(); m_pidRL.reset(); m_pidRR.reset();
}

void Navigator::update(RobotState state) {
    // Tầng Cao: Cập nhật định vị tọa độ
    m_odometry.update(state.tickFL, state.tickFR, state.tickRL, state.tickRR);
}

void Navigator::execute(RobotCommand cmd) {
    // 1. KINEMATICS: Dịch lệnh (vx, vy, omega) thành vận tốc mục tiêu cho 4 bánh
    float targetFL, targetFR, targetRL, targetRR;
    m_kinematics.calculateWheelSpeeds(cmd.vx, cmd.vy, cmd.omega, 
                                      targetFL, targetFR, targetRL, targetRR);

    // 2. XUẤT PWM (Chế độ Open-Loop an toàn để test Motor)
    // Giả sử target (từ Joystick) chạy từ -100 đến 100, ta nhân 2.55 để ra xung PWM -255 đến 255
    float pwmFL = targetFL * 2.55f;
    float pwmFR = targetFR * 2.55f;
    float pwmRL = targetRL * 2.55f;
    float pwmRR = targetRR * 2.55f;

    /* --- CHẾ ĐỘ CLOSED-LOOP PID (MỞ LÊN KHI ĐÃ GẮN ENCODER THẬT) ---
    float dt = 0.01f; // Chu kỳ Task_Control là 10ms
    float pwmFL = m_pidFL.compute(targetFL, currentSpeedFL, dt);
    float pwmFR = m_pidFR.compute(targetFR, currentSpeedFR, dt);
    float pwmRL = m_pidRL.compute(targetRL, currentSpeedRL, dt);
    float pwmRR = m_pidRR.compute(targetRR, currentSpeedRR, dt);
    */

    // 3. Gửi tín hiệu PWM đến Motor Driver
    //motorDriver.setSpeeds(pwmFL, pwmFR, pwmRL, pwmRR);

    // 4. In ra Serial để nghiệm thu
    if (cmd.vx != 0 || cmd.vy != 0 || cmd.omega != 0) {
        Serial.printf("[NAV] PWM -> FL:%.0f | FR:%.0f | RL:%.0f | RR:%.0f\n", 
                      pwmFL, pwmFR, pwmRL, pwmRR);
    }
}