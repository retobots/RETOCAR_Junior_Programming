# KIẾN TRÚC FIRMWARE: RETOCAR JOYSTICK CONTROLLER

## 1. PHÂN TẦNG PHẦN MỀM (MODULAR STACK)
Hệ thống được tổ chức thành 3 lớp riêng biệt để đảm bảo tính dễ bảo trì và khả năng mở rộng:
* **Lớp Ứng dụng (Application Layer):** Quản lý logic điều hướng xe, máy trạng thái giao diện (UI State Machine) và xử lý dữ liệu người dùng.
* **Lớp Dịch vụ (Service Layer):** Xử lý truyền thông ESP-NOW, quản lý bộ nhớ NVS (Flash) để lưu trữ các hệ số $K_p$, $K_i$, $K_d$.
* **Lớp Driver (HAL Layer):** Điều khiển trực tiếp phần cứng gồm LovyanGFX (TFT), ADC1 (Joystick), và GPIO (Button/Buzzer).

## 2. QUẢN LÝ ĐA TÁC VỤ TRÊN DUAL-CORE (FREERTOS)
Việc chuyển sang ESP32-WROOM cho phép chúng ta phân bổ công việc lên 2 nhân để triệt tiêu tình trạng giật lag màn hình khi đang truyền nhận dữ liệu:

| Tác vụ (Task) | Nhân (Core) | Chu kỳ | Ưu tiên | Vai trò chính |
| :--- | :---: | :---: | :---: | :--- |
| **Task_Input** | Core 1 | 20ms | 5 | Đọc ADC Joystick, tính toán Dead-zone và hàm mũ Expo. |
| **Task_Radio** | Core 0 | 30ms | 4 | Quản lý ESP-NOW. Gửi lệnh điều khiển và kiểm tra Failsafe. |
| **Task_LVGL** | Core 0 | 5ms | 3 | Thực thi `lv_timer_handler()`. Duy trì đồ họa mượt mà. |
| **Task_UI_Sync** | Core 0 | 100ms | 2 | Cập nhật Telemetry từ xe lên Dashboard (Pin, Tọa độ, IMU). |

## 3. XỬ LÝ NHẬP LIỆU & ĐIỀU KHIỂN CHUYÊN SÂU
Để hỗ trợ việc điều khiển xe Mecanum tinh tế, lớp Driver Input được lập trình các thuật toán bổ trợ:
* **ADC1 Priority:** Chỉ sử dụng bộ ADC1 (VP, VN, D34, D35) để không bị ngắt quãng khi bật WiFi/ESP-NOW.
* **Dead-zone (5%):** Loại bỏ nhiễu tại điểm tâm của Joystick, ngăn xe tự trôi khi không chạm vào cần gạt.
* **Exponential Mapping:** Áp dụng hàm bậc 3 cho vận tốc xoay $\omega$. Điều này giúp học viên có thể xoay xe cực chậm để căn chỉnh tọa độ hoặc xoay cực nhanh khi cần né vật cản.

## 4. QUẢN LÝ DỮ LIỆU & ĐỒNG BỘ (DATA SYNC)
Do dữ liệu được ghi ở Core 1 nhưng lại được gửi đi và hiển thị ở Core 0, hệ thống sử dụng các cơ chế an toàn:
* **Mutex (Semaphore):** Bảo vệ biến currentControl. Khi Core 1 đang ghi dữ liệu vận tốc mới, Core 0 (Radio) phải đợi để tránh gửi đi các gói tin bị lỗi dữ liệu.
* **Queue:** Sử dụng hàng đợi để nhận dữ liệu Telemetry từ xe về, đảm bảo không mất thông tin cảm biến khi màn hình đang bận xử lý đồ họa.

## 5. MÁY TRẠNG THÁI GIAO DIỆN (UI STATE MACHINE)
Sử dụng LVGL để quản lý 4 trạng thái màn hình chính:
* **BOOT:** Kiểm tra kết nối với xe. Nếu không thấy xe, hiển thị thông báo lỗi ngay lập tức.
* **DASHBOARD:** Màn hình hoạt động chính với la bàn số, đồ thị vận tốc và tọa độ $(x, y)$ thực tế.
* **TUNING:** Menu điều chỉnh hệ số PID. Dữ liệu thay đổi sẽ được gửi gói tin Configuration sang xe để lưu vào Flash.
* **WARNING:** Cửa sổ Pop-up ưu tiên cao nhất, tự động hiện đè lên khi khoảng cách siêu âm báo vật cản $< 10cm$ hoặc mất kết nối $> 500ms$.

## 6. CÁC CƠ CHẾ AN TOÀN (LOGIC FAILSAFE)
* **Watchdog Telemetry:** Nếu sau 1 giây không nhận được gói tin phản hồi từ xe, toàn bộ UI sẽ chuyển sang màu đỏ và phát tín hiệu Buzzer liên tục.
* **DMA for SPI:** Cấu hình LovyanGFX sử dụng bộ đệm DMA để đẩy dữ liệu lên màn hình, giải phóng CPU cho các tác vụ tính toán vận tốc ở Core 1.

---

# KIẾN TRÚC CHƯƠNG TRÌNH STM32

## 1. PHÂN TẦNG KIẾN TRÚC (SOFTWARE LAYERS)
Để đảm bảo tính thời gian thực và độ tin cậy của dữ liệu phản hồi (Feedback), chương trình được chia làm 3 lớp xử lý:
* **Lớp Driver (Hardware Abstraction Layer):** Cấu hình trực tiếp các thanh ghi ngoại vi.
  * **Timers (TIM1, 2, 3, 4):** Cấu hình chế độ Encoder Mode để đếm xung phần cứng.
  * **ADC:** Đọc điện áp Pin (VBAT) qua mạch phân áp.
  * **GPIO:** Đọc 05 mắt dò Line và điều khiển LED RGB/Xi-nhan.
  * **UART + DMA:** Truyền dữ liệu tốc độ cao không gây nghẽn CPU.
* **Lớp Service (Middleware):** Xử lý và làm sạch dữ liệu.
  * **Encoder Manager:** Quản lý giá trị tích lũy của 4 bộ đếm, xử lý hiện tượng tràn số (Overflow).
  * **Filter Service:**
    * Áp dụng thuật toán Majority Voting để lọc Cảm biến Line để tránh các tín hiệu ảo từ ánh sáng môi trường.
    * Áp dụng thuật toán Median Filter (Lọc trung vị) kết hợp Threshold Checking cho Encoder.
    * Áp dụng thuật toán Trung bình động lũy thừa (EMA) cho đọc điện áp Pin.
  * **Protocol Manager:** Đóng gói dữ liệu cảm biến kèm mã kiểm tra lỗi CRC8.
* **Lớp Application (Task Dispatcher):** Điều phối hoạt động theo chu kỳ. Đảm bảo việc đẩy dữ liệu lên Master diễn ra đúng chu kỳ (10ms - 20ms).

## 2. THIẾT KẾ MÁY TRẠNG THÁI (FINITE STATE MACHINE)
Trong vai trò mới, máy trạng thái của Slave được tối giản hóa để tập trung vào việc báo cáo thông tin:
* **STATE_INIT (Khởi tạo):**
  * Khởi động 4 Timer Encoder, xóa các bộ đếm về 0.
  * Khởi tạo UART DMA ở chế độ Idle Line để sẵn sàng nhận lệnh từ Master.
* **STATE_ACTIVE (Thu thập & Báo cáo):**
  * **Công việc định kỳ (10ms):** Đọc xung 4 bánh, đọc trạng thái 5 mắt Line, đọc điện áp Pin.
  * **Truyền thông:** Đẩy gói tin Telemetry lên Master.
  * **Thực thi lệnh:** Cập nhật màu LED RGB dựa trên yêu cầu từ Master.
* **STATE_FAULT (Báo lỗi):**
  * Kích hoạt khi mất kết nối UART với Master > 500ms hoặc điện áp Pin dưới ngưỡng an toàn.
  * Đưa toàn bộ tín hiệu báo hiệu về trạng thái cảnh báo (LED Đỏ nháy nhanh).

## 3. QUẢN LÝ NGOẠI VI (PERIPHERAL CONFIGURATION)
Để đáp ứng tốc độ phản hồi cần thiết cho thuật toán Odometry trên Master, các ngoại vi được cấu hình chuyên sâu:
* **Encoder (Hardware Timers):**
  * Sử dụng cả 2 kênh (Combined Channels) của Timer để đạt độ phân giải tối đa (x4).
  * Bật bộ lọc nhiễu Input Filter để loại bỏ nhiễu điện từ phát ra từ chổi than của motor TT.
* **UART2 + DMA (Communication):**
  * **TX DMA:** Master yêu cầu dữ liệu rất thường xuyên (100Hz), do đó việc dùng DMA là bắt buộc để tránh làm trôi nhịp đọc Encoder.
  * **RX DMA (Idle Line):** Nhận lệnh điều khiển LED/Buzzer từ Master một cách linh hoạt mà không cần biết trước độ dài gói tin.
* **ADC (Battery Monitor):**
  * Sử dụng chế độ Scan hoặc ngắt ADC để cập nhật mức pin mỗi 100ms.

## 4. GIAO THỨC TRUYỀN THÔNG (SLAVE PUSH MODEL)
* Slave không đợi Master hỏi mà chủ động đẩy dữ liệu theo nhịp thời gian cố định. Điều này cực kỳ quan trọng để Master tính toán vận tốc bánh xe một cách chính xác.
* Cấu trúc gói tin Telemetry (Slave → Master) - Dùng Struct:
  * **Header:** 0xAA (Nhận diện gói tin).
  * **Encoder Data:** 4 biến int32_t chứa giá trị tích lũy của 4 bánh.
  * **Line Data:** 1 biến uint8_t (mỗi bit đại diện cho 1 mắt dò line).
  * **Voltage:** 1 biến uint16_t (giá trị ADC pin).
  * **Checksum:** CRC8 (Kiểm tra toàn vẹn dữ liệu).

## 5. LƯU Ý KỸ THUẬT KHI LẬP TRÌNH
* **Xử lý đảo hướng:** Do các motor Mecanum có hướng lắp đặt khác nhau, Slave nên gửi giá trị xung thô. Việc quy định hướng tiến/lùi nên để Master xử lý trong thuật toán Kinematics.
* **Chống treo UART:** Cần triển khai hàm `HAL_UART_ErrorCallback` để tự động khởi động lại kênh DMA nếu có nhiễu điện từ làm lỗi khung truyền.

---

# KIẾN TRÚC FIRMWARE: MAIN BOARD ESP32

## 1. PHÂN TẦNG ĐIỀU KHIỂN (CASCADED CONTROL ARCHITECTURE)
* **Tầng Cao (Navigation Layer):**
  * **Input:** Tọa độ đích (Auto Pose) hoặc Vị trí vạch (Auto Line).
  * **Feedback:** Odometry (Encoder + IMU) hoặc Trạng thái 5 mắt Line.
  * **Output:** Vận tốc mục tiêu tổng quát của xe (V_x, V_y, omega).
* **Tầng Thấp (Velocity Layer):**
  * **Input:** Tốc độ mục tiêu cho từng bánh (tính từ Động học nghịch).
  * **Feedback:** Tốc độ thực tế từ Encoder (do STM32 gửi lên).
  * **Output:** Giá trị PWM trực tiếp cho Driver DRV8833.

## 2. PHÂN BỔ TÁC VỤ TRÊN DUAL-CORE (FREERTOS STRATEGY)
Tận dụng tối đa 2 nhân của ESP32 để triệt tiêu độ trễ truyền thông ảnh hưởng đến điều khiển:
* 🟢 **Core 1: The Control Heart (Ưu tiên 5 - Cao nhất)**
  * **Task_Control (10ms):**
    * Đọc cảm biến siêu âm (qua ngắt External Interrupt).
    * Tính toán Odometry & PID Tầng cao.
    * Tính toán Kinematics & PID Tầng thấp.
    * Xuất Hardware PWM (LEDC Driver).
* 🔵 **Core 0: The Communication Hub (Ưu tiên 4 & 3)**
  * **Task_Comm_Slave (10ms):** Nhận và giải mã gói tin UART DMA từ STM32. Cập nhật vào vùng nhớ dùng chung.
  * **Task_Radio (30ms):** Quản lý ESP-NOW. Nhận lệnh từ Joystick và gửi dữ liệu Telemetry (Vị trí, Pin, Cảm biến) về cho người dùng.
  * **Task_System (100ms):** Quản lý máy trạng thái, kiểm tra điện áp Pin và thực hiện logic bảo vệ (Failsafe).

## 3. MÁY TRẠNG THÁI HỆ THỐNG (SYSTEM STATE MACHINE)
Kiến trúc firmware cần quản lý các chuyển đổi trạng thái một cách chặt chẽ:
* **MODE_MANUAL:** Chạy theo lệnh trực tiếp từ Joystick. Áp dụng Exponential Mapping để xe xoay mượt mà.
* **MODE_AUTO_LINE:** Kích hoạt bộ PID Line-Follower. Lấy dữ liệu 5 mắt line từ vùng nhớ dùng chung làm Feedback.
* **MODE_AUTO_POSE:** Kích hoạt bộ PID Navigation. Xe tự di chuyển đến tọa độ $(x, y)$ mong muốn.
* **STATE_EMERGENCY:** Trạng thái dừng cưỡng bức. Kích hoạt ngay lập tức khi Siêu âm báo vật cản hoặc lệnh dừng từ Joystick.

## 4. QUẢN LÝ DỮ LIỆU DÙNG CHUNG (SHARED DATA STRUCTURE)
Để 2 Core "nói chuyện" với nhau mà không bị xung đột, chúng ta sử dụng một cấu trúc dữ liệu trung tâm:
* **Struct RobotState:** Lưu trữ Tọa độ hiện tại, Góc hướng, Trạng thái 5 mắt line, Khoảng cách vật cản và Điện áp pin.
* **Struct RobotCommand:** Lưu trữ Vận tốc mục tiêu và Chế độ hoạt động (Mode).
* **Cơ chế:** Sử dụng Mutex (Semaphore) để bảo vệ các Struct này khi Task_Control và Task_Comm_Slave truy cập cùng lúc.

## 5. CƠ CHẾ AN TOÀN (SAFETY LAYER)
* **Hardware Stop:** Siêu âm sử dụng ngắt chân Echo, tác động trực tiếp vào biến dừng trong Task_Control để dừng xe trong $< 50ms$.
* **Communication Watchdog:** Nếu Core 0 không nhận được dữ liệu từ Joystick hoặc Slave trong một khoảng thời gian (ví dụ 500ms), nó sẽ tự động ghi lệnh dừng vào RobotCommand.
