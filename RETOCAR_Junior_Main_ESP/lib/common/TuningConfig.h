#ifndef TUNING_CONFIG_H
#define TUNING_CONFIG_H

#define ROBOT_LX            0.08f   // Khoảng cách trục dọc (Tâm đến trục bánh trước/sau)
#define ROBOT_LY            0.08f   // Khoảng cách trục ngang (Tâm đến trục bánh trái/phải)
#define ROBOT_WHEEL_RADIUS  0.03f   // Bán kính bánh xe Mecanum

#define ENCODER_TICKS_PER_REV 330.0f // Số xung đếm được khi bánh xe quay 1 vòng

#define PID_KP_DEFAULT      1.0f    // Hệ số Tỉ lệ
#define PID_KI_DEFAULT      0.1f    // Hệ số Tích phân
#define PID_KD_DEFAULT      0.01f   // Hệ số Vi phân
#define PID_OUT_MAX         255.0f  // Giới hạn xung PWM tối đa (Tiến)
#define PID_OUT_MIN        -255.0f  // Giới hạn xung PWM tối thiểu (Lùi)

#endif