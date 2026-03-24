/*
 * robot_slave.cpp
 *
 *  Created on: Mar 17, 2026
 *      Author: retobots
 */
#include "Robot_slave.h"

RobotSlave::RobotSlave() : currentState(STATE_INIT) {}

// void RobotSlave::init(UART_HandleTypeDef *huart, ADC_HandleTypeDef *hadc)
// {
//     // Khởi tạo các module cấp thấp
//     // encoder.begin(); // Start 4 Timer Hardware Encoder
//     // lineSensor.begin();
//     // protocol.begin(huart); // Init UART DMA

//     // currentState = ACTIVE;
// }

// void RobotSlave::update()
// {
//     switch (currentState)
//     {
//     case STATE_ACTIVE:
//         handleActiveState();
//         break;

//     case FAULT:
//         // Xử lý lỗi (Dừng truyền, nháy LED đỏ)
//         break;

//     default:
//         break;
//     }
// }

// void RobotSlave::handleActiveState()
// {
//     // 1. Thu thập dữ liệu thô (Raw Data)
//     RawData raw;
//     raw.encoderData = encoder.readAll();
//     raw.lineData = lineSensor.readAll();
//     raw.batteryVoltage = HAL_ADC_GetValue(...); // Đọc ADC Pin

//     // 2. Xử lý qua Filter Service (Dữ liệu "Sạch")
//     // Sử dụng Majority Voting cho Line, Median cho Encoder, EMA cho Pin
//     CleanData clean = filter.process(raw);

//     // 3. Đóng gói và gửi qua Master (UART DMA)
//     // RobotSlave không quan tâm cách gửi, chỉ giao Data cho ProtocolManager
//     protocol.sendTelemetry(clean);

//     // 4. Kiểm tra lệnh từ Master (Giao tiếp 2 chiều)
//     if (protocol.hasNewCommand())
//     {
//         Command cmd = protocol.getCommand();
//         // Thực thi lệnh (Ví dụ: Đổi màu LED RGB)
//         // LED_SetColor(cmd.rgbColor);
//     }
// }
