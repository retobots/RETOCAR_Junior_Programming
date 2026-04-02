/**
 * @file Application.cpp
 * @brief Triển khai vòng lặp điều khiển ứng dụng chính và thu thập ngoại vi.
 */
#include "Application.hpp"
#include "AppStateManager.hpp"
#include "EncoderManager.hpp"
#include "ProtocolManager.hpp"
#include "Common/DataStructs.h"

#include "stm32f1xx_hal.h"
#include "main.h"

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern ADC_HandleTypeDef hadc1;
extern UART_HandleTypeDef huart3;

/* Biến khả dụng toàn cục cho mục tiêu Debug */
AppStateManager   g_stateManager;
EncoderManager    g_encFrontLeft(500); 
EncoderManager    g_encFrontRight(500);
EncoderManager    g_encRearLeft(500);
EncoderManager    g_encRearRight(500);

TelemetryPacket   g_telemetryPacket;

void AppMain_Init(void) {
    g_stateManager.init();
    g_encFrontLeft.init();
    g_encFrontRight.init();
    g_encRearLeft.init();
    g_encRearRight.init();
    
    /* Thiết lập chân GPIO PC13 đóng vai trò đèn báo hiệu dữ liệu truyền UART */
    __HAL_RCC_GPIOC_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /* Chạy hiệu chuẩn ADC phần cứng để triệt tiêu Offset và tăng độ tin cậy của nhiễu nền */
    HAL_ADCEx_Calibration_Start(&hadc1);

    /* Kích hoạt chế độ đọc trực tiếp Quadrature Encoder Mode (x4) cho các Timer */
    HAL_TIM_Encoder_Start(&htim1, TIM_CHANNEL_ALL);
    HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL);
    HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);
    HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_ALL);

    /* Dòng tín hiệu giả lập Watchdog đầu tiên để mở khoá trạng thái máy */
    g_stateManager.feedWatchdog(HAL_GetTick()); 
}

void AppMain_Loop(void) {
    uint32_t currentTick = HAL_GetTick();

    /* 1. Lấy mẫu liên tục mật độ Pulse nhịp đập từ cơ cấu chấp hành (Encoders) */
    uint16_t c_FL = __HAL_TIM_GET_COUNTER(&htim1);
    uint16_t c_RR = __HAL_TIM_GET_COUNTER(&htim2);
    uint16_t c_RL = __HAL_TIM_GET_COUNTER(&htim3);
    uint16_t c_FR = __HAL_TIM_GET_COUNTER(&htim4);

    g_encFrontLeft.update(c_FL);
    g_encFrontRight.update(c_FR);
    g_encRearLeft.update(c_RL);
    g_encRearRight.update(c_RR);

    /* 2. Xử lý điện áp cung cấp và giao thức kiểm soát lỗi ADC */
    static float lastValidBatteryVoltage = 8.0f;
    float realBatteryVoltage = lastValidBatteryVoltage; 
    
    /* Chống dội / ngập Interrupt do xung đột nhịp baudrate truyền thông UART */
    __HAL_UART_CLEAR_OREFLAG(&huart3);

    /* Tự động phục hồi máy trạng thái ADC nếu phần cứng bị khoá kẹt (HAL Error Recovery) */
    if (hadc1.State == HAL_ADC_STATE_ERROR_INTERNAL) {
        hadc1.State = HAL_ADC_STATE_READY;
    }

    HAL_ADC_Start(&hadc1);
    if (HAL_ADC_PollForConversion(&hadc1, 5) == HAL_OK) { 
        uint32_t rawAdcVal = HAL_ADC_GetValue(&hadc1);
        /* Giải thuật hồi quy điện áp thực dựa vào phân áp cấu hình R1=100kΩ, R2=47kΩ */
        realBatteryVoltage = (float)rawAdcVal * (3.3f / 4095.0f) * (147.0f / 47.0f); 
        lastValidBatteryVoltage = realBatteryVoltage;
    }
    HAL_ADC_Stop(&hadc1);

    /* 3. Tích hợp Máy Quản Trị Trạng Thái Sinh Tồn (State Manager) */
    g_stateManager.update(currentTick, realBatteryVoltage);

    if (g_stateManager.getCurrentState() == AppState::STATE_FAULT) {
        /* Thiết lập mô hình nhấp nháy LED khẩn cấp khi hệ thống mất khả năng hoạt động */
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_10);
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_11);
    }

    /* 4. Truyền thông đa phương thức khi Hệ thống Hoạt Động (ACTIVE) */
    if (g_stateManager.getCurrentState() == AppState::STATE_ACTIVE) {
        
        g_telemetryPacket.encoderFrontLeft = g_encFrontLeft.getAccumulatedValue();
        g_telemetryPacket.encoderFrontRight = g_encFrontRight.getAccumulatedValue();
        g_telemetryPacket.encoderRearLeft = g_encRearLeft.getAccumulatedValue();
        g_telemetryPacket.encoderRearRight = g_encRearRight.getAccumulatedValue();
        
        g_telemetryPacket.batteryVoltage = (uint16_t)(realBatteryVoltage * 100.0f);
        
        /* Chuyển hóa 5 dải cảm biến Digital rời rạc thành chuỗi Bitmap gọn nhẹ */
        uint8_t lineStatus = 0;
        if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12)) lineStatus |= (1 << 0);
        if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13)) lineStatus |= (1 << 1);
        if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14)) lineStatus |= (1 << 2);
        if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_15)) lineStatus |= (1 << 3);
        if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_12)) lineStatus |= (1 << 4);
        g_telemetryPacket.lineSensors = lineStatus;
        
        ProtocolManager::packageTelemetry(g_telemetryPacket);

        /* Thực thi chuyển giao mã nhị phân thông minh qua UART về bộ Master Node */
        HAL_StatusTypeDef status = HAL_UART_Transmit(&huart3, (uint8_t*)&g_telemetryPacket, sizeof(TelemetryPacket), 10);
        
        if (status == HAL_OK) {
            static uint8_t blinkCounter = 0;
            blinkCounter++;
            if (blinkCounter >= 50) {
                HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13); 
                blinkCounter = 0;
            }
        }
    }

    /* 5. Cấp nhịp duy trì sự sống (Heartbeat Cycle) */
    g_stateManager.feedWatchdog(currentTick);
}
