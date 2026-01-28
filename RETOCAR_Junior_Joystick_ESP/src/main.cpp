#include <Arduino.h>
#include "data_structs.h"
#include "display_driver.h"
#include "joystick_driver.h"
#include "esp_now_service.h"
#include "ui_manager.h"

// --- Khai báo Task Handles ---
TaskHandle_t TaskInputHandle = NULL;
TaskHandle_t TaskRadioHandle = NULL;
TaskHandle_t TaskUIHandle = NULL;

// --- Khai báo tài nguyên dùng chung ---
ControlPacket currentControl;      // Dữ liệu điều khiển hiện tại
TelemetryPacket latestTelemetry;   // Dữ liệu xe gửi về mới nhất
SemaphoreHandle_t xMutexData;      // Bảo vệ dữ liệu dùng chung

// --- Nguyên mẫu các Task ---
void TaskInput(void *pvParameters);
void TaskRadio(void *pvParameters);
void TaskUI(void *pvParameters);

void setup() {
    Serial.begin(115200);

    // 1. Khởi tạo tài nguyên hệ thống
    xMutexData = xSemaphoreCreateMutex();
    
    // 2. Khởi tạo phần cứng (Drivers)
    // Khởi tạo TFT + LovyanGFX bằng đối tượng display_driver
    // Khởi tạo ADC cho cần điều khiển bằng đối tượng joystick_driver
    // Khởi tạo thư viện LVGL bằng đối tượng ui_manager
    
    // 3. Khởi tạo dịch vụ truyền thông
    // Cấu hình ESP-NOW bằng đối tượng esp_now_service

    // 4. Khởi tạo giao diện (UI)
    // Vẽ màn hình chính bằng đối tượng ui_manager

    // 5. Tạo các tác vụ FreeRTOS (Tối ưu cho ESP32-C3 đơn nhân)
    xTaskCreate(TaskInput, "InputTask", 2048, NULL, 4, &TaskInputHandle);
    xTaskCreate(TaskRadio, "RadioTask", 3072, NULL, 3, &TaskRadioHandle);
    xTaskCreate(TaskUI,    "UITask",    4096, NULL, 2, &TaskUIHandle);
}

void loop() {
    // Để trống vì đã sử dụng FreeRTOS Tasks
    vTaskDelete(NULL);
}

// --- CHI TIẾT CÁC TÁC VỤ ---

// Tác vụ 1: Đọc Joystick & Nút nhấn (Chu kỳ 20ms)
void TaskInput(void *pvParameters) {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    while (1) {
        if (xSemaphoreTake(xMutexData, portMAX_DELAY)) {
            // Đọc và xử lý toán học (Deadzone, Exponential Mapping) và lưu vào currentControl
            xSemaphoreGive(xMutexData);
        }
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(20));
    }
}

// Tác vụ 2: Truyền thông ESP-NOW (Chu kỳ 30ms)
void TaskRadio(void *pvParameters) {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    while (1) {
        if (xSemaphoreTake(xMutexData, portMAX_DELAY)) {
            // Gửi dữ liệu lệnh điều khiển từ currentControl sang xe sử dụng đối tượng esp_now_service
            
            // Cập nhật dữ liệu Telemetry nhận được từ Callback thuộc esp_now_service vào latestTelemetry
            xSemaphoreGive(xMutexData);
        }
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(30));
    }
}

// Tác vụ 3: Cập nhật Giao diện LVGL (Chu kỳ 5ms - 50ms)
void TaskUI(void *pvParameters) {
    while (1) {
        // Cập nhật bộ đệm LVGL (Bắt buộc chạy liên tục) sử dụng ui_manager
        
        // Cập nhật số liệu trên màn hình mỗi 100ms để tránh lag
        static uint32_t lastUpdate = 0;
        if (millis() - lastUpdate > 100) {
            // đọc latestTelemetry và gọi hàm cập nhật màn hình thuộc đối tượng ui_manager
            lastUpdate = millis();
        }
        
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}