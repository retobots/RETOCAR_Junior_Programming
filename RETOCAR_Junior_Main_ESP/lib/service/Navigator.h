#ifndef NAVIGATOR_H
#define NAVIGATOR_H

#include "Kinematics.h"
#include "Odometry.h"
#include "PIDController.h"
#include "DataStructs.h"

class Navigator {
private:
    Kinematics m_kinematics;
    Odometry m_odometry;

    PIDController m_pidFL;
    PIDController m_pidFR;
    PIDController m_pidRL;
    PIDController m_pidRR;

public:
    Navigator();

    void init();

    // Cập nhật dữ liệu từ Cảm biến & Encoder
    void update(RobotState state);

    // Xử lý Lệnh từ Joystick và xuất PWM
    void execute(RobotCommand cmd);
};

extern Navigator navigator;

#endif