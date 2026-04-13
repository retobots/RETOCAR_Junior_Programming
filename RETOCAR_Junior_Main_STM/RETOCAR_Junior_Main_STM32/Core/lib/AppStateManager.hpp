/**
 * @file AppStateManager.hpp
 * @brief Đơn vị đánh giá và xử lý Máy Trạng Thái Cảnh Giác (Safety State Machine).
 */
#ifndef APP_STATE_MANAGER_HPP
#define APP_STATE_MANAGER_HPP

#include <stdint.h>

/**
 * @enum AppState
 * @brief Chỉ thị các phân vùng hoạt động an toàn của vi điều khiển.
 */
enum class AppState {
    STATE_INIT,     /**< Phân vùng khởi tạo cấu hình mức nhân */
    STATE_ACTIVE,   /**< Phân vùng hoạt động đa nhiệm thông thường */
    STATE_FAULT     /**< Phân vùng tạm phong toả lỗi rủi ro cao (Low Voltage / Watchdog) */
};

class AppStateManager {
private:
    AppState m_currentState;                  /**< Trạng thái chấp hành hiện tại của máy */
    uint32_t m_lastMasterComUpdateMs;         /**< Tem thời gian (Timestamp) cuối cùng giữ liên lạc */
    float m_smoothedBatteryVoltage;           /**< Dòng điện áp Pin sau khi được làm phẳng thông qua EMA */
    
    static const uint32_t TIMEOUT_MS = 500;           /**< Ngưỡng thời gian định sẵn để tuyên bố mất sóng (500ms) */
    static constexpr float LOW_BATT_THRESHOLD = 6.2f; /**< Ngưỡng điện áp cắt tải cứng của Pin Lipo 2 Cell (6.2V) */

public:
    AppStateManager();

    /**
     * @brief Kích hoạt hệ thống và tải giá trị thiết lập ban đầu.
     */
    void init();

    /**
     * @brief Vòng lặp liên tục quét các biến số ảnh hưởng đến rủi ro sập nguồn robot.
     * @param currentTimeMs Biến đếm Timer thời gian độc lập.
     * @param rawBatteryVoltage Mẫu số học lấy về từ Module chuyển đổi Analog-Digital.
     */
    void update(uint32_t currentTimeMs, float rawBatteryVoltage);

    /**
     * @brief Reset cơ chế bảo vệ theo dõi sinh tồn (Watchdog Protocol).
     * @param currentTimeMs Tem thời gian tại cột mốc giao tiếp thành công.
     */
    void feedWatchdog(uint32_t currentTimeMs);

    /**
     * @brief Trích xuất trạng thái phong toả / giải phóng hiện thời của vi điều khiển.
     * @return AppState Thuộc tính trạng thái (enum).
     */
    AppState getCurrentState() const;
    
private:
    /**
     * @brief Trình xử lý hành vi cơ điện khi xâm nhập vùng an toàn (Fault State).
     */
    void handleFaultAction();
};

#endif // APP_STATE_MANAGER_HPP
