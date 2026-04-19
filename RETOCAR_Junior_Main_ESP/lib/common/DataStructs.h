#ifndef DATA_STRUCTS_H
#define DATA_STRUCTS_H

#include <Arduino.h>
#include <HardwareSerial.h>

struct RobotCommand {
    int8_t vx;
    int8_t vy;  
    int8_t omega;
    uint8_t mode;
    uint8_t buzzer;
} __attribute__((packed));

struct RobotState {
    float posX;
    float posY;
    float heading;

    uint8_t lineL2;
    uint8_t lineL1;
    uint8_t lineC;
    uint8_t lineR1;
    uint8_t lineR2;
    
    float distance;
    uint16_t battery;

    int32_t tickFL;
    int32_t tickFR;
    int32_t tickRL;
    int32_t tickRR;
    
} __attribute__((packed));

#endif