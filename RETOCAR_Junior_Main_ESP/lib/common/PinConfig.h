#ifndef PIN_CONFIG_H
#define PIN_CONFIG_H

// --- MPU6050 I2C Pins ---
#define I2C_SDA_PIN 21
#define I2C_SCL_PIN 22

// --- Ultrasonic HC-SR04 Pins ---
#define HC_SR04_TRIG_PIN 4
#define HC_SR04_ECHO_PIN 2

// --- Motor Control Pins (DRV8833) ---
// Front Left
#define PIN_MOTOR_FL_IN1 32
#define PIN_MOTOR_FL_IN2 33

// Front Right
#define PIN_MOTOR_FR_IN1 25
#define PIN_MOTOR_FR_IN2 26

// Rear Left
#define PIN_MOTOR_RL_IN1 18
#define PIN_MOTOR_RL_IN2 19

// Rear Right
#define PIN_MOTOR_RR_IN1 17
#define PIN_MOTOR_RR_IN2 5

#endif // PIN_CONFIG_H
