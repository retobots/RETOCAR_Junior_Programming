# GIT CONVENTIONS - RETOCAR JUNIOR

## 1. Cấu trúc thư mục (Monorepo Layout)
Để Git phân loại tốt, cấu trúc thư mục phải cực kỳ rạch ròi. Các kỹ sư không được phép để code "lạc trôi" ngoài các thư mục định danh:

```plaintext
RETOCAR_JUNIOR_PROGRAMMING/ (Root)
├── .gitignore                     # File chặn rác tổng
├── RETOCAR_Junior_Main_STM/       # Code STM32 (Smart Sensor Hub)
├── RETOCAR_Junior_Main_ESP/       # Code ESP32 (Master - Control Heart)
└── RETOCAR_Junior_Joystick_ESP/   # Code ESP32 (Joystick Controller)
```

## 2. Quy tắc Commit với "Scope" (Bắt buộc)
Trong Monorepo, nhìn vào lịch sử commit mà chỉ thấy `feat: update PID` thì không ai biết là PID của board nào. Quy tắc Conventional Commits cần bổ sung thêm **Scope**:
* **Cấu trúc:** `<type>(<scope>): <mô tả>`

| Scope | Đối tượng áp dụng | Ví dụ Commit Message |
| :--- | :--- | :--- |
| **main_stm** | Main Board - STM32 | `feat(main_stm): thêm bộ lọc Median cho encoder` |
| **main_esp** | Main Board - ESP32 | `fix(main_esp): sửa lỗi trễ Task_Control trên Core 1` |
| **ctrl** | Controller Board | `feat(ctrl): thiết kế lại dashboard LVGL` |

## 3. Chiến lược phân nhánh (Branching Strategy)
Vì 3 dự án chạy song song, việc đặt tên branch phải chứa định danh project để tránh xung đột khi Merge Request:
* **Format:** `feature/[scope]/[tên-tính-năng]`
* **Ví dụ:**
  * `feature/main_stm/adc-dma-filter`
  * `feature/main_esp/odometry-optimization`
  * `feature/ctrl/esp-now-failsafe`
* **Quy tắc Merge:** Khi một kỹ sư sửa lỗi ở `RETOCAR_Junior_Main_ESP`, họ chỉ được phép sửa trong thư mục đó. Nếu một PR (Pull Request) sửa "lan" sang thư mục khác mà không báo trước, PR sẽ bị từ chối (Reject) ngay để tránh lỗi dây chuyền.

## 4. Quy tắc gắn thẻ (Tagging & Releases)
Vì 3 board có thể có tốc độ nâng cấp khác nhau, chúng ta không dùng một version chung. Hãy sử dụng **Prefix Tag**:
* **STM32 Slave:** `stm-v1.0.2`
* **ESP32 Main:** `main-v1.1.0`
* **ESP32 Controller:** `ctrl-v1.0.5`

Việc này giúp biết chính xác bộ firmware nào đang chạy ổn định với nhau (ví dụ: `main-v1.1.0` phải đi kèm với `stm-v1.0.2` thì mới hiểu giao thức UART mới).

## 5. Checklist cho kỹ sư (Review Rule)
Khi duyệt PR cho Monorepo, hãy tự đặt 3 câu hỏi:
1. "Kỹ sư này có dùng đúng Scope trong commit message không?"
2. "Thay đổi này có làm thay đổi `Common/DataStructs.h` không? (Nếu có, phải báo cho các team khác cập nhật ngay)."
3. "Kỹ sư code cho STM32 có lỡ tay xóa nhầm file của ESP32 không?"

---

# CODING CONVENTIONS - RETOCAR JUNIOR

## 1. Quy tắc đặt tên (Naming Conventions)
Mục tiêu: Đọc tên biến/hàm là biết ngay chức năng và phạm vi sử dụng.
* **Class (Lớp):** Sử dụng `PascalCase`.
  * *Ví dụ:* `MotorDriver`, `PidController`, `SlaveComm`.
* **Hàm (Functions/Methods):** Sử dụng `camelCase`.
  * *Ví dụ:* `readEncoder()`, `calculatePid()`, `startMotor()`.
* **Biến cục bộ & Tham số:** Sử dụng `camelCase`.
  * *Ví dụ:* `targetSpeed`, `currentError`, `deltaTime`.
* **Biến thành viên của Class (Private):** Thêm tiền tố `m_` và dùng `camelCase`.
  * *Ví dụ:* `m_kp`, `m_prevError`, `m_sampleTime`.
* **Hằng số & Macro:** Sử dụng `SNAKE_UPPERCASE`.
  * *Ví dụ:* `MAX_PWM`, `WHEEL_RADIUS`, `PID_INTERVAL_MS`.
* **File:** Tên file phải trùng với tên Class bên trong.
  * *Ví dụ:* `Navigator.hpp`, `Navigator.cpp`.

## 2. Cấu trúc File & OOP
* **Header Guard:** Luôn sử dụng `#ifndef`, `#define`, `#endif` để tránh lỗi include nhiều lần.
* **Encapsulation (Đóng gói):** Tất cả thuộc tính (Variables) phải để là `private` hoặc `protected`. Truy cập qua các hàm `get...()` hoặc `set...()`.
* **Null Pointer:** Sử dụng `nullptr` thay cho `NULL`.
* **Đơn vị đo:** Luôn ghi chú đơn vị nếu biến liên quan đến vật lý.
  * *Ví dụ:* `float distance_mm`, `uint32_t interval_ms`.

## 3. Quy tắc FreeRTOS & Đa nhân (ESP32 Specific)
Đây là phần quan trọng nhất để hệ thống không bị "treo" hoặc chạy sai PID.
* **Core Affinity:**
  * Tuyệt đối không chạy các hàm chặn (blocking) như `delay()` hoặc `Serial.print()` trong các Task thuộc Core 1 (nhân xử lý điều khiển).
  * Tác vụ liên quan đến WiFi/ESP-NOW phải nằm ở Core 0.
* **Thread Safety (An toàn dữ liệu):**
  * Mọi biến dùng chung giữa 2 Task hoặc 2 Core phải được bảo vệ bởi `SemaphoreHandle_t` (Mutex).
  * Nguyên tắc: "Lock -> Read/Write -> Unlock" nhanh nhất có thể.
* **Task Loop:** Sử dụng `vTaskDelayUntil()` thay cho `vTaskDelay()` cho các vòng lặp PID để đảm bảo chu kỳ không bị trôi (Jitter).

## 4. Quản lý Phần cứng (Hardware Safety)
* **Define Pin:** Không "hard-code" số chân GPIO vào trong code. Tất cả phải nằm trong `PinConfig.h`.
* **Failsafe:** Trong các hàm điều khiển động cơ, luôn có kiểm tra điều kiện an toàn (ví dụ: nếu `targetSpeed > MAX_SPEED` thì gán bằng `MAX_SPEED`).
* **Comment (Chú thích):**
  * Giải thích *Tại sao* làm vậy (Logic), không giải thích *Cái gì* (Code đã thể hiện rồi).
  * Mỗi Class/Hàm quan trọng phải có mô tả ngắn về Input và Output.

## 5. Ví dụ về một đoạn code chuẩn:

```cpp
// File: PidController.hpp
#ifndef PID_CONTROLLER_HPP
#define PID_CONTROLLER_HPP

class PidController {
private:
    float m_kp;           // Hệ số tỉ lệ
    float m_prevError;    // Sai số lần trước để tính Vi phân
    const float m_limit;  // Giới hạn đầu ra (Anti-windup)

public:
    PidController(float kp, float limit);
    float compute(float setpoint, float feedback, float dt);
};

#endif
```

## 6. Checklist trước khi Pull Request (Giao việc cho kỹ sư)
1. Code đã compile thành công và không có Warning?
2. Các biến dùng chung đã có Mutex bảo vệ chưa?
3. Task điều khiển có bị vướng hàm `delay()` nào không?
4. Tên biến/hàm đã tuân thủ Pascal/camelCase chưa?
5. Đã cập nhật các thông số PID vào file cấu hình chưa?
