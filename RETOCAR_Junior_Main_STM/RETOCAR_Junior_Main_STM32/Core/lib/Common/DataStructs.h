#ifndef DATA_STRUCTS_H
#define DATA_STRUCTS_H

#include <stdint.h>

#pragma pack(push, 1)

//STM32 gửi lên ESP32
struct TelemetryPacket {
    uint8_t header;             // Byte nhận diện (0xAA)
    int32_t encoderFrontLeft;   // Xung tích lũy bánh trước trái
    int32_t encoderFrontRight;  // Xung tích lũy bánh trước phải
    int32_t encoderRearLeft;    // Xung tích lũy bánh sau trái
    int32_t encoderRearRight;   // Xung tích lũy bánh sau phải
    uint8_t lineSensors;        // Trạng thái 5 mắt dò Line
    uint16_t batteryVoltage;    // Giá trị điện áp pin
    uint8_t checksum;           // CRC8 của các byte trước
};

// ESP32 gửi xuống STM32
struct CommandPacket {
    uint8_t header;         // Byte nhận diện (0x55)
    uint8_t ledStatus;      // Trạng thái dàn đèn LED 
    uint8_t systemConfig;   // Lệnh cấu hình 
    uint8_t checksum;       // CRC8 của các byte trước
};

#pragma pack(pop)

#define TELEMETRY_HEADER 0xAA
#define COMMAND_HEADER   0x55

#endif
