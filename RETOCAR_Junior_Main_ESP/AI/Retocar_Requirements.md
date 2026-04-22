# BẢN MÔ TẢ YÊU CẦU TỔNG THỂ: HỆ THỐNG RETOCAR JUNIOR PRO

## 1. GIỚI THIỆU MỤC TIÊU
RETOCAR Junior Pro là nền tảng robot tự hành nâng cao, tập trung vào việc làm chủ các thuật toán **Định vị (Odometry)**, **Điều khiển phản hồi (PID)** và **Động học Mecanum**. Hệ thống được thiết kế theo kiến trúc điều khiển phân tán để tối ưu hóa hiệu suất và độ chính xác của cảm biến.

## 2. KIẾN TRÚC PHẦN CỨNG XE (VEHICLE HARDWARE)

### 2.1. Master (Trung tâm điều hành): ESP32-WROOM-32 (30-pin)
* **Vai trò:** Là "não bộ" duy nhất đưa ra quyết định di chuyển và thực thi lệnh.
* **Điều khiển động cơ:** Trực tiếp xuất 4 kênh Hardware PWM (Tốc độ) và 4 chân GPIO (Hướng) cho 2 mạch DRV8833. Việc điều khiển trực tiếp giúp loại bỏ hoàn toàn độ trễ truyền thông.
* **Cảm biến an toàn:** Kết nối trực tiếp Siêu âm (HC-SR04) để xử lý ngắt và phản xạ né vật cản tức thì.
* **Định hướng:** Kết nối IMU MPU6050 qua I2C để lấy dữ liệu góc hướng (Yaw) phục vụ bù trừ sai số cơ khí.
* **Mở rộng:** 01 RC Servo và 01 Buzzer.

### 2.2. Slave (Trạm cảm biến thông minh): STM32F103C8T6
* **Vai trò:** Là "giác quan" chuyên dụng, thu thập dữ liệu thô và báo cáo về Master.
* **Encoder:** Tận dụng tối đa 4 Timer phần cứng (TIM1-TIM4) để đọc 4 bộ Encoder AB. Đảm bảo độ phân giải cao nhất và không mất xung ở mọi tốc độ.
* **Dò Line:** Đọc trạng thái 05 mắt cảm biến hồng ngoại.
* **Báo hiệu:** Nhận lệnh từ Master để điều khiển LED RGB (mức logic 0/1) nhằm hiển thị trạng thái hệ thống.

### 2.3. Hệ thống Nguồn
* **Nguồn:** Pin Li-ion 2S (7.4V - 8.4V).
* **Cấu trúc:** Loại bỏ mạch sạc nội vi. Sử dụng bộ sạc ngoài chuyên dụng.
* **An toàn:** Tích hợp Diode chống ngược cực và mạch phân áp đọc điện áp Pin (ADC).

## 3. KIẾN TRÚC BOARD ĐIỀU KHIỂN (JOYSTICK CONTROLLER)

### 3.1. Phần cứng
* **MCU:** ESP32-WROOM-32.
* **Hiển thị:** Màn hình TFT SPI 2.4 inch, sử dụng thư viện LovyanGFX và LVGL.
* **Input:** 02 Cụm Joystick Analog (3-axis control) và 02 nút nhấn tùy biến.
* **Phản hồi:** 01 Buzzer phát tín hiệu cảnh báo mất kết nối hoặc va chạm.

### 3.2. Chức năng UI (LVGL Screens)
* **Dashboard:** Hiển thị tọa độ thực thời (x, y), hướng xe, tốc độ 4 bánh và dung lượng Pin xe.
* **Tuning Mode:** Giao diện cho phép tinh chỉnh trực tiếp K_p, K_i, K_d và lưu vào Flash của ESP32 trên xe thông qua ESP-NOW.

## 4. KIẾN TRÚC PHẦN MỀM (SOFTWARE ARCHITECTURE)

### 4.1. Master (Hệ điều hành chính)
* **Phương pháp:** FreeRTOS (Dual-core) kết hợp System State Machine.
* **Quản lý trạng thái (Brain):**
  * `STATE_MANUAL`: Nhận lệnh từ Joystick, tính toán động học nghịch (Inverse Kinematics) và xuất PWM.
  * `STATE_AUTO_LINE`: Lấy dữ liệu mắt Line từ Slave, chạy thuật toán PID bám vạch.
  * `STATE_AUTO_POINT`: Sử dụng dữ liệu Encoder từ Slave để chạy Odometry đi đến tọa độ đích.
  * `STATE_EMERGENCY`: Dừng khẩn cấp khi cảm biến Siêu âm báo vật cản hoặc mất kết nối RF.
* **Phân bổ Core:** Core 1 xử lý tính toán PID/PWM (10ms); Core 0 xử lý ESP-NOW và UART Communication.

### 4.2. Slave (Xử lý dữ liệu thô)
* **Phương pháp:** Interrupt-Driven (Hướng ngắt).
* **Cơ chế:** Cứ mỗi 10ms - 20ms, Slave đóng gói dữ liệu số xung 4 bánh và trạng thái 5 mắt Line thành một struct và đẩy lên Master qua UART DMA.

### 4.3. Joystick (Giao diện người dùng)
* **Xử lý:** Task UI chạy LVGL độc lập; Task Input đọc ADC và áp dụng bộ lọc Exponential Mapping để điều khiển xe mượt mà hơn ở dải tốc độ thấp.

## 5. GIAO THỨC TRUYỀN THÔNG (PROTOCOLS)
* **ESP-NOW (Không dây):** Joystick gửi lệnh vận tốc và thông số PID; Master gửi lại dữ liệu Telemetry (tọa độ, pin, khoảng cách vật cản).
* **UART (Nội bộ):** Slave gửi dữ liệu Encoder/Line; Master gửi lệnh điều khiển LED RGB/Buzzer.

## 6. TIÊU CHÍ NGHIỆM THU (ACCEPTANCE CRITERIA)
* **Chính xác:** Xe đi thẳng 2m, sai số lệch ngang không quá 3cm.
* **Định vị:** Xe xoay tại chỗ 360 độ, sai số góc hướng sau khi xoay < 1 độ (nhờ kết hợp IMU).
* **An toàn:** Phản ứng dừng khẩn cấp với vật cản thực hiện trong vòng 50ms.
* **HMI:** Giao diện Joystick cập nhật thông tin xe ổn định ở mức 20-30 FPS, không gây lag lệnh điều khiển.

---

# BẢN MÔ TẢ YÊU CẦU LẬP TRÌNH: RETOCAR JOYSTICK CONTROLLER

## 1. MÔI TRƯỜNG PHÁT TRIỂN & THƯ VIỆN
* **Vi điều khiển:** ESP32-WROOM-32 (Dual-core để tách biệt tác vụ UI và Truyền thông).
* **Hệ điều hành:** FreeRTOS.
* **Thư viện đồ họa (UI):**
  * **LVGL (v8.3+):** Quản lý Widget, đồ thị và menu.
  * **LovyanGFX:** Thư viện Driver tốc độ cao cho màn hình TFT SPI 2.4 inch.
* **Giao thức không dây:** ESP-NOW (Chế độ Full-duplex: Gửi lệnh và nhận Telemetry).

## 2. QUẢN LÝ NHẬP LIỆU (JOYSTICK & BUTTONS)
Yêu cầu xử lý dữ liệu thô từ ADC1 (để tránh xung đột với WiFi/ESP-NOW):
* **Hiệu chuẩn (Calibration):** Tự động xác định điểm Zero khi khởi động.
* **Vùng chết (Dead-zone):** Thiết lập ngưỡng 5% quanh điểm Zero để tránh xe bị trôi nhẹ.
* **Tính toán Vận tốc:**
  * **Vx, Vy:** Ánh xạ (Map) trực tiếp từ tọa độ Joystick sang dải [-100, 100].
  * **Vận tốc xoay (ω):** Sử dụng Hàm số mũ (Exponential Mapping). Khi gạt nhẹ xe xoay chậm để quan sát, khi gạt mạnh xe xoay nhanh.

## 3. CẤU TRÚC ĐA TÁC VỤ (FREERTOS TASKS)
Hệ thống được chia thành 3 Task chính để đảm bảo hiệu suất:
* **Task UI (Core 0):** Chạy `lv_timer_handler()` để cập nhật giao diện và phản hồi cảm ứng/nút nhấn.
* **Task Input (Core 1):** Đọc ADC Joystick mỗi 20ms, tính toán vận tốc tổng quát và đóng gói dữ liệu.
* **Task Comms (Core 0):** Quản lý truyền nhận ESP-NOW. Gửi lệnh điều khiển mỗi 30ms và nhận Telemetry từ xe.

## 4. GIAO DIỆN NGƯỜI DÙNG (LVGL GUI)
Thiết kế với 3 màn hình chính (Screens):
* **Màn hình Dashboard:** Hiển thị tọa độ thực thời (x, y), hướng xe và tốc độ 4 bánh. Visualizer: Icon xe xoay và di chuyển tương ứng theo dữ liệu Odometry gửi về. Thanh trạng thái Pin xe và cường độ tín hiệu.
* **Màn hình Diagnostic:** Trạng thái 5 mắt dò Line (Đen/Trắng). Khoảng cách vật cản từ cảm biến siêu âm (Progress bar).
* **Màn hình Tuning:** Giao diện chỉnh sửa thông số K_p, K_i, K_d. Cơ chế: Khi thay đổi, dữ liệu được gửi ngay sang xe và lưu vào Flash/NVS.

## 5. GIAO THỨC TRUYỀN THÔNG (ESP-NOW)
Dữ liệu đóng gói dạng struct:
* **ControlPacket (Gửi đi):** vx, vy, omega, mode, buzzer.
* **TelemetryPacket (Nhận về):** posX, posY, heading, battery, sensors, distance.

## 6. LOGIC AN TOÀN (FAIL-SAFE)
* **Mất kết nối:** Nếu không nhận được tín hiệu từ xe trong 500ms, hiển thị cảnh báo "LOST CONNECTION" và phát tiếng Buzzer.
* **Hỗ trợ người dùng:** Buzzer phát tín hiệu khi xe báo va chạm hoặc pin yếu.

---

# BẢN MÔ TẢ YÊU CẦU LẬP TRÌNH: RETOCAR JUNIOR MAIN BOARD

## 1. KIẾN TRÚC PHẦN MỀM TỔNG QUÁT
Hệ thống hoạt động theo mô hình Distributed Real-time Control:
* **Master (ESP32-WROOM-32):** Là trung tâm điều hành (Brain), quản lý Máy trạng thái hệ thống, thuật toán PID/Odometry, truyền thông không dây và trực tiếp thực thi các tác vụ động lực/an toàn.
* **Slave (STM32F103C8T6):** Là trạm cảm biến thông minh (Smart Sensor Hub), thực hiện các tác vụ thu thập dữ liệu thô yêu cầu độ chính xác về thời gian thực từ phần cứng.
* **Giao thức kết nối:** UART2 với Baudrate 115200 hoặc cao hơn.

## 2. NHIỆM VỤ CỦA SLAVE (STM32F103C8T6)
Sử dụng Máy trạng thái (State Machine) và hướng ngắt (Interrupt-driven) để xử lý dữ liệu.

### 2.1. Đọc Encoder độ phân giải cao
* Cấu hình đồng thời 4 bộ Timer phần cứng (TIM1, TIM2, TIM3, TIM4) ở chế độ Encoder Mode.
* Đảm bảo việc đọc xung 2 pha A-B không bị mất xung ngay cả khi xe quay ở tốc độ tối đa (416rpm).

### 2.2. Thu thập dữ liệu cảm biến
* **Dò Line:** Đọc trạng thái Digital của 05 mắt hồng ngoại.
* **Giám sát Pin:** Đọc ADC và báo cáo điện áp pin về Master.

### 2.3. Thực thi báo hiệu
* Nhận lệnh từ Master để điều khiển LED RGB và hệ thống đèn xi-nhan theo mức logic 0/1.

### 2.4. Đẩy dữ liệu định kỳ (Data Pushing)
* Cứ mỗi 10ms - 20ms, Slave đóng gói dữ liệu (số xung 4 bánh, trạng thái 5 mắt Line, điện áp pin) thành một struct và đẩy lên Master qua UART DMA để giảm tải cho CPU.

## 3. NHIỆM VỤ CỦA MASTER (ESP32-WROOM-32)
Sử dụng FreeRTOS (Dual-core) để tối ưu hóa hiệu năng tính toán.

### 3.1. Phân bổ Core xử lý
* **Core 1 (Control Task - 10ms):**
  * Trực tiếp xuất Hardware PWM điều khiển 4 động cơ.
  * Đọc cảm biến Siêu âm và xử lý ngắt để dừng xe ngay lập tức khi gặp vật cản.
  * Đọc IMU MPU6050 qua I2C để lấy góc hướng (Yaw).
  * Thực hiện thuật toán PID & Odometry dựa trên số xung nhận được từ Slave.
  * Điều khiển RC Servo và Buzzer.
* **Core 0 (Communication Task):**
  * Quản lý kết nối ESP-NOW với Joystick (Nhận lệnh vận tốc/PID, gửi Telemetry).
  * Xử lý nhận gói tin UART từ Slave và giải mã Checksum.

### 3.2. Quản lý trạng thái hệ thống (State Machine)
* `STATE_MANUAL`: Di chuyển theo lệnh Joystick.
* `STATE_AUTO_LINE`: Dò line dựa trên dữ liệu 5 mắt Line từ Slave.
* `STATE_AUTO_POINT`: Tự di chuyển đến tọa độ đích (x, y).
* `STATE_EMERGENCY`: Chế độ an toàn (Dừng xe, hú còi) khi phát hiện vật cản hoặc mất kết nối RF.

## 4. GIAO THỨC TRUYỀN THÔNG UART (MASTER ↔ SLAVE)
Dữ liệu trao đổi theo cấu trúc gói tin (Struct) kèm CRC8 Checksum.
* **Slave → Master:** `[Header][Length][4x Encoder Data][Line Status][Battery Voltage][CRC8]`
* **Master → Slave:** `[Header][Length][LED RGB State][Buzzer/Indicator Command][CRC8]`

## 5. TIÊU CHÍ NGHIỆM THU PHẦN MỀM
* **Độ trễ:** Xe phản hồi lệnh từ Joystick với độ trễ < 50ms.
* **Phản xạ an toàn:** Phản ứng dừng khẩn cấp với vật cản thực hiện trong vòng 50ms.
* **Độ chính xác:** Sai số định vị Odometry không quá 3cm trên quãng đường 2m.
* **Độ ổn định:** Hệ thống chạy liên tục không bị treo (Deadlock) giữa 2 MCU và không mất xung Encoder phần cứng.
