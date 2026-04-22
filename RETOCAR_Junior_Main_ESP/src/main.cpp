#include <Arduino.h>
#include "PinConfig.h"
#include "DataStructs.h"
#include "MotorDriver.h"
#include "SlaveComm.h"
#include "RadioService.h"
#include "Navigator.h"

#include "IMUDriver.h"
#include "UltrasonicDriver.h"

IMUDriver imu;
UltrasonicDriver ultrasonic;

RobotState g_robotState;
RobotCommand g_robotCmd;

// --- Task Handles ---
TaskHandle_t hControlTask, hCommSlaveTask, hRadioTask, hSystemTask;
SemaphoreHandle_t xMutexData;

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
  // slaveComm.begin();
  // radioService.begin();
  // navigator.init();

  if (imu.begin()) {
    Serial.println("MPU6050 initialized successfully!");
  } else {
    Serial.println("Failed to initialize MPU6050!");
  }

  ultrasonic.begin();
  Serial.println("Ultrasonic initialized.");

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
    // Kích hoạt siêu âm bắn xung
    ultrasonic.trigger();

    // Đọc dữ liệu từ driver
    float pitch, roll, yaw;
    imu.readPRY(pitch, roll, yaw);
    float dist = ultrasonic.getDistanceCm();

    if (xSemaphoreTake(xMutexData, portMAX_DELAY))
    {

      g_robotState.distance = dist;
      g_robotState.heading = yaw;
      
      // Lấy Feedback đã được Task_Comm_Slave cập nhật sẵn
      // navigator.update(g_robotState);
      // // Tính toán PID & Xuất PWM
      // navigator.execute(g_robotCmd);
      xSemaphoreGive(xMutexData);
    }

    static int pCounter = 0;
    if (++pCounter >= 10) {
      Serial.printf("Distance: %.2f cm | Pitch: %.2f, Roll: %.2f, Yaw: %.2f\n", 
                    dist, pitch, roll, yaw);
      pCounter = 0;
    }
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(10));
  }
}

// TASK 2: Giải mã UART từ STM32 (10ms)
void Task_Comm_Slave(void *pv)
{
  while (1)
  {
    // // SlaveComm xử lý UART DMA ở background, Task này chỉ bóc tách struct
    // if (slaveComm.available())
    // {
    //   if (xSemaphoreTake(xMutexData, pdMS_TO_TICKS(2)))
    //   {
    //     g_robotState.encoders = slaveComm.getEncoders();
    //     g_robotState.lineSensors = slaveComm.getLine();
    //     xSemaphoreGive(xMutexData);
    //   }
    // }
    vTaskDelay(pdMS_TO_TICKS(10));
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