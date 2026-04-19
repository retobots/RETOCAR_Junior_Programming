#include <Arduino.h>
#include "PinConfig.h"
#include "DataStructs.h"
#include "MotorDriver.h"
#include "SlaveComm.h"
#include "RadioService.h"
#include "Navigator.h"

// --- Task Handles ---
TaskHandle_t hControlTask, hCommSlaveTask, hRadioTask, hSystemTask;
SemaphoreHandle_t xMutexData;

RobotState g_robotState;
RobotCommand g_robotCmd;

// --- Task Prototypes ---
void Task_Control(void *pv);    // Core 1 - 10ms: PID & Hardware Output
void Task_Comm_Slave(void *pv); // Core 0 - 10ms: UART DMA from STM32
void Task_Radio(void *pv);      // Core 0 - 30ms: ESP-NOW with Joystick
void Task_System(void *pv);     // Core 0 - 100ms: State Machine & Safety

void setup()
{
  Serial.begin(115200);
  xMutexData = xSemaphoreCreateMutex();

  // Khởi tạo Drivers & Middleware
  // motorDriver.begin();
  slaveComm.begin();
  // radioService.begin();
  // navigator.init();

  // --- PHÂN BỔ 4 TASKS THEO THIẾT KẾ ---

  // 1. Task Điều khiển (Nhân 1 - Ưu tiên cao nhất)
  xTaskCreatePinnedToCore(Task_Control, "Control", 8192, NULL, 5, &hControlTask, 1);

  // 2. Task Nhận dữ liệu Slave (Nhân 0 - Cập nhật Feedback liên tục)
  xTaskCreatePinnedToCore(Task_Comm_Slave, "Slave", 4096, NULL, 4, &hCommSlaveTask, 0);

  // 3. Task Sóng Radio (Nhân 0 - Nhận lệnh điều khiển từ xa)
  xTaskCreatePinnedToCore(Task_Radio, "Radio", 4096, NULL, 3, &hRadioTask, 0);

  // 4. Task Hệ thống (Nhân 0 - Quản lý logic chậm & Failsafe)
  xTaskCreatePinnedToCore(Task_System, "System", 4096, NULL, 2, &hSystemTask, 0);
}

void loop() { vTaskDelete(NULL); }

// =================================================================
// CHI TIẾT CÁC TÁC VỤ
// =================================================================

// TASK 1: Xử lý toán học & Xuất xung (10ms)
void Task_Control(void *pv)
{
  TickType_t xLastWakeTime = xTaskGetTickCount();
  while (1)
  {
    if (xSemaphoreTake(xMutexData, portMAX_DELAY))
    {
      // Lấy Feedback đã được Task_Comm_Slave cập nhật sẵn
      // navigator.update(g_robotState);
      // // Tính toán PID & Xuất PWM
      // navigator.execute(g_robotCmd);
      xSemaphoreGive(xMutexData);
    }
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(10));
  }
}

// TASK 2: Giải mã UART từ STM32 (10ms)
void Task_Comm_Slave(void *pv)
{
  TickType_t xLastWakeTime = xTaskGetTickCount();
  const TickType_t xInterval = pdMS_TO_TICKS(10); // Check UART mỗi 10ms

  while (1)
  {
    // Cầm riêng 1 Object để nhào nặn Data UART, không làm tắc nghẽn Semaphone của người khác
    RobotState tempState;

    if (slaveComm.readData(tempState))
    {
      // Chỉ khi tín hiệu báo Data Payload cực dạch bóc thành công, ta mới xin chìa khoá Mutex gán Memory
      if (xSemaphoreTake(xMutexData, pdMS_TO_TICKS(3)))
      {
        g_robotState.tickFL = tempState.tickFL;
        g_robotState.tickFR = tempState.tickFR;
        g_robotState.tickRL = tempState.tickRL;
        g_robotState.tickRR = tempState.tickRR;

        g_robotState.lineL2 = tempState.lineL2;
        g_robotState.lineL1 = tempState.lineL1;
        g_robotState.lineC  = tempState.lineC;
        g_robotState.lineR1 = tempState.lineR1;
        g_robotState.lineR2 = tempState.lineR2;

        g_robotState.battery = tempState.battery;
        
        // // ---Đặt dữ liệu tĩnh để STM32 chớp tín hiệu led nhịp ---
        // g_robotCmd.mode = 1;   
        // g_robotCmd.buzzer = 0; 
        
        // --- DEBUG SERIAL: Kểm tra xem có nhận được dữ liệu không ---
        // static uint8_t debugCounter = 0;
        // if (++debugCounter >= 50) {
        //     Serial.printf("[DEBUG UART] Encoders: %d | %d | %d | %d -- Line: %d %d %d %d %d -- Pin: %d mV\n",
        //         g_robotState.tickFL, g_robotState.tickFR, g_robotState.tickRL, g_robotState.tickRR,
        //         g_robotState.lineL2, g_robotState.lineL1, g_robotState.lineC, g_robotState.lineR1, g_robotState.lineR2,
        //         g_robotState.battery);
        //     debugCounter = 0;
        // }

        xSemaphoreGive(xMutexData);
      }
    }

    // Đẩy tiếp cấu hình mới nhất hiện tại xuống mạch STM32 bằng lệnh sendCommand
    RobotCommand currentCmd;
    if (xSemaphoreTake(xMutexData, pdMS_TO_TICKS(2))) {
        currentCmd = g_robotCmd;
        xSemaphoreGive(xMutexData);
    }
    slaveComm.sendCommand(currentCmd); // Bắn ngược lại UART

    vTaskDelayUntil(&xLastWakeTime, xInterval);
  }
}

// TASK 3: Giao tiếp ESP-NOW (30ms)
void Task_Radio(void *pv)
{
  while (1)
  {
    // if (radioService.receive())
    // {
    //   if (xSemaphoreTake(xMutexData, pdMS_TO_TICKS(5)))
    //   {
    //     g_robotCmd = radioService.getCmd();
    //     xSemaphoreGive(xMutexData);
    //   }
    // }
    // // Gửi Telemetry về Joystick
    // radioService.send(g_robotState);
    vTaskDelay(pdMS_TO_TICKS(30));
  }
}

// TASK 4: Giám sát an toàn & State Machine (100ms)
void Task_System(void *pv)
{
  while (1)
  {
    // // Kiểm tra Pin, kiểm tra mất kết nối (Watchdog)
    // systemManager.checkFailsafe();
    // // Chuyển đổi trạng thái Manual/Auto
    // systemManager.updateMode();

    vTaskDelay(pdMS_TO_TICKS(100));
  }
}