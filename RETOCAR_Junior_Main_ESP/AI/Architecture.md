# 🤖 Project Architecture: RETOCAR_Junior_Main_ESP

## 📌 Tổng quan dự án (Project Overview)
- **Nền tảng:** ESP32 (Master) điều khiển Robot, giao tiếp với STM32 (Slave).
- **Framework:** PlatformIO (Arduino/ESP-IDF).
- **Hệ điều hành:** FreeRTOS (Quản lý đa tác vụ).
- **Kiến trúc phần mềm:** Phân lớp (Layered Architecture) bao gồm: App (Logic cấp cao), Service (Thuật toán), Driver (Phần cứng) và Common (Cấu hình chung).

---

## 📂 Cấu trúc thư mục chi tiết (Directory Structure)

### 1. `lib/common/` (Tầng Cấu hình & Dữ liệu chung)
Tầng này chứa các file định nghĩa, hằng số và cấu trúc dữ liệu dùng chung cho toàn bộ dự án.
- **`DataStructs.h`**: Chứa định nghĩa `struct`, `enum`, `union` (VD: Gói tin Payload truyền UART Master-Slave, cấu trúc dữ liệu cảm biến).
- **`PinConfig.h`**: Định nghĩa toàn bộ chân GPIO của ESP32 (Macro `#define`). Không hard-code chân ở các file khác.
- **`TuningConfig.h`**: Chứa các thông số hằng số cấu hình (Hệ số PID Kp/Ki/Kd, chu vi bánh xe, khoảng cách 2 bánh, ngưỡng cảm biến siêu âm...).

### 2. `lib/driver/` (Tầng Giao tiếp Phần cứng - Hardware Layer)
Tầng này làm việc trực tiếp với ngoại vi và tín hiệu vật lý. Không chứa thuật toán phức tạp ở đây.
- **`IMUDriver`**: Giao tiếp I2C đọc dữ liệu từ cảm biến gia tốc/góc nghiêng (MPU6050/BNO055). Trả về góc Roll, Pitch, Yaw.
- **`MotorDriver`**: Điều khiển PWM cấp xung cho driver động cơ (L298N/TB6612). Chạy tiến, lùi, dừng.
- **`RadioService`**: Giao tiếp sóng RF (NRF24L01 / LoRa / RC Receiver) để nhận lệnh điều khiển từ tay cầm.
- **`SlaveComm`**: Giao tiếp UART (Non-blocking) với STM32 Slave. Đóng gói/Giải mã bản tin, tính toán CRC8 bảo mật.
- **`UltrasonicDriver`**: Đọc cảm biến khoảng cách siêu âm (HC-SR04/JSN-SR04T) bằng Timer/Interrupt.

### 3. `lib/service/` (Tầng Thuật toán & Xử lý số liệu - Middleware Layer)
Tầng này nhận dữ liệu thô từ `driver`, xử lý toán học và trả ra kết quả để tầng `app` sử dụng.
- **`Kinematics`**: Động học Robot. Chuyển đổi từ vận tốc tuyến tính/góc (V, Omega) ra vận tốc từng bánh xe (Forward/Inverse Kinematics).
- **`Navigator`**: Thuật toán dẫn đường (Line tracking, Obstacle avoidance, State Machine né vật cản).
- **`Odometry`**: Tính toán tọa độ vị trí thực tế của robot (X, Y, Theta) dựa trên tick encoder gửi lên từ STM32.
- **`PIDController`**: Thuật toán PID độc lập. Được gọi để bám line, cân bằng hoặc giữ tốc độ bánh xe.

<!-- ### 4. `lib/app/` (Tầng Ứng dụng & Điều hành - High-Level Layer)
Tầng cao nhất quản lý luồng chạy của toàn bộ Robot.
- **`RobotMaster`**: Quản lý State Machine trung tâm của xe (Chế độ Manual, Auto Line, Auto Avoid, Stop). Gọi các Service và Driver để kết nối hệ thống.
- **`TaskManager`**: Nơi khởi tạo và quản lý các Task của FreeRTOS (Task UART, Task Sensor, Task Control, Task Radio). Phân bổ độ ưu tiên (Priority) và dung lượng RAM (Stack size). -->