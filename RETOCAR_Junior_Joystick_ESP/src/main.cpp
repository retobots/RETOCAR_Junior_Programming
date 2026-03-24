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
TaskHandle_t TaskUISyncHandle = NULL;

// --- Khai báo tài nguyên dùng chung ---
ControlPacket currentControl;    // Dữ liệu điều khiển gửi đi
TelemetryPacket latestTelemetry; // Dữ liệu xe gửi về
SemaphoreHandle_t xMutexData;    // Bảo vệ dữ liệu dùng chung giữa các Core

// --- Nguyên mẫu các Task ---
void TaskInput(void *pvParameters);  // Core 1: Đọc ADC, xử lý toán học
void TaskRadio(void *pvParameters);  // Core 0: Giao tiếp không dây
void TaskUI(void *pvParameters);     // Core 0: Duy trì LVGL Tick
void TaskUISync(void *pvParameters); // Core 0: Cập nhật Telemetry lên màn hình

void setup()
{
    Serial.begin(115200);

    // 1. Khởi tạo tài nguyên hệ thống
    xMutexData = xSemaphoreCreateMutex();

    // 2. Khởi tạo phần cứng (Drivers)
    // Các đối tượng này đã được cấu hình chân theo Schematic V2 (D23, D22, D18, D16, D4...)
    // display_driver.begin();
    // joystick_driver.begin(); // Cấu hình ADC1 cho VP, VN, D34, D35
    // ui_manager.begin();      // Khởi tạo LVGL và các widget Dashboard, Tuning

    // 3. Khởi tạo dịch vụ truyền thông
    // esp_now_service.begin();

    // 4. Tạo các Task chạy song song
    // Task Input: Ưu tiên cao nhất, chạy trên Core 1 để tránh trễ lệnh lái
    xTaskCreatePinnedToCore(TaskInput, "TaskInput", 4096, NULL, 5, &TaskInputHandle, 1);

    // Các tác vụ truyền thông và đồ họa chạy trên Core 0
    xTaskCreatePinnedToCore(TaskRadio, "TaskRadio", 4096, NULL, 4, &TaskRadioHandle, 0);
    xTaskCreatePinnedToCore(TaskUI, "TaskUI", 8192, NULL, 3, &TaskUIHandle, 0);
    xTaskCreatePinnedToCore(TaskUISync, "TaskUISync", 4096, NULL, 2, &TaskUISyncHandle, 0);
}

void loop()
{
    // FreeRTOS quản lý các Task, loop() không cần thực hiện gì
    vTaskDelete(NULL);
}

// --- CHI TIẾT CÁC TÁC VỤ ---

// Tác vụ 1: Xử lý Input (Core 1 - 20ms)
void TaskInput(void *pvParameters)
{
    TickType_t xLastWakeTime = xTaskGetTickCount();
    ControlPacket raw;
    while (1)
    {
        /* đọc joystick thông qua Control_Data_Service */
        joystick_read(&raw); 
        if (xSemaphoreTake(xMutexData, portMAX_DELAY))
        {
            /* DEADZONE */
            int vx = applyDeadzone(raw.vx, 100);
            int vy = applyDeadzone(raw.vy, 100);
            int omega = applyDeadzone(raw.omega, 100);

            /* NORMALIZE */
            vx = normalizeAxis(vx);
            vy = normalizeAxis(vy);
            omega = normalizeAxis(omega);

            /* EXPO cho trục xoay */
            float w = (float)omega / 100.0f;
            w = applyExpo(w, 0.3f);

            /* Ghi vào control packet */
            currentControl.vx = vx;
            currentControl.vy = vy;
            currentControl.omega = (int)(w * 100);

            currentControl.mode = raw.mode;  // Kiểm tra mode (nút nhấn)
            
            xSemaphoreGive(xMutexData);
        }

        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(20));  // Đọc mỗi 20ms
    }
}

// Tác vụ 2: Truyền thông ESP-NOW (Core 0 - 30ms)
void TaskRadio(void *pvParameters)
{
    TickType_t xLastWakeTime = xTaskGetTickCount();
    while (1)
    {
        if (xSemaphoreTake(xMutexData, pdMS_TO_TICKS(10)))
        {
            // Gửi dữ liệu điều khiển sang xe
            // esp_now_service.send(currentControl);

            // Cập nhật Telemetry từ xe vào latestTelemetry qua callback
            // latestTelemetry = esp_now_service.getLatestTelemetry();
            xSemaphoreGive(xMutexData);
        }
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(30));
    }
}

// Tác vụ 3: Duy trì LVGL (Core 0 - 5ms)
void TaskUI(void *pvParameters)
{
    while (1)
    {
        // lv_timer_handler(); // Bắt buộc chạy liên tục để duy trì hoạt ảnh
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}

// Tác vụ 4: Đồng bộ dữ liệu lên Dashboard (Core 0 - 100ms)
void TaskUISync(void *pvParameters)
{
    while (1)
    {
        if (xSemaphoreTake(xMutexData, portMAX_DELAY))
        {
            // Cập nhật các Widget: Pin, Tọa độ X-Y, Góc hướng, Khoảng cách siêu âm
            // ui_manager.updateDashboard(latestTelemetry);
            xSemaphoreGive(xMutexData);
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}