/**
 * @file AppStateManager.cpp
 * @brief Điều phối chính sách cắt tải tự động dựa vào logic giám sát nguồn điện.
 */
#include "AppStateManager.hpp"
#include "FilterService.hpp"

AppStateManager::AppStateManager() 
    : m_currentState(AppState::STATE_INIT), 
      m_lastMasterComUpdateMs(0), 
      m_smoothedBatteryVoltage(8.4f)
{
}

void AppStateManager::init() {
    m_currentState = AppState::STATE_INIT;
    m_lastMasterComUpdateMs = 0;
}

void AppStateManager::update(uint32_t currentTimeMs, float rawBatteryVoltage) {
    /* Bộ lọc tần số thấp (EMA) cho phép nhận diện mức sụt áp thật thay vì nhiễu cơ học */
    m_smoothedBatteryVoltage = FilterService::applyEma(rawBatteryVoltage, m_smoothedBatteryVoltage, 0.1f);

    switch (m_currentState) {
        case AppState::STATE_INIT:
            /* Thông quan trạng thái khởi động mượt mà để sẵn sàng làm việc */
            m_currentState = AppState::STATE_ACTIVE;
            break;

        case AppState::STATE_ACTIVE:
            /* Nếu robot không nhận lệnh Quá Hạn hoặc Lõi năng lượng quá thấp, cắt tải tự động */
            if ((currentTimeMs - m_lastMasterComUpdateMs > TIMEOUT_MS) || 
                (m_smoothedBatteryVoltage < LOW_BATT_THRESHOLD)) {
                
                m_currentState = AppState::STATE_FAULT;
            }
            break;

        case AppState::STATE_FAULT:
            handleFaultAction();
            
            /* Kỹ thuật ranh giới độ trễ (Hysteresis Window) cộng thêm (+0.2V) 
               để tránh hiệu ứng bật tắt liên tục (Oscillations) khi chạm ngưỡng pin. */
            if ((currentTimeMs - m_lastMasterComUpdateMs <= TIMEOUT_MS) && 
                (m_smoothedBatteryVoltage >= LOW_BATT_THRESHOLD + 0.2f)) {
                
                m_currentState = AppState::STATE_ACTIVE;
            }
            break;
    }
}

void AppStateManager::feedWatchdog(uint32_t currentTimeMs) {
    m_lastMasterComUpdateMs = currentTimeMs;
}

AppState AppStateManager::getCurrentState() const {
    return m_currentState;
}

void AppStateManager::handleFaultAction() {
    /* Đóng vai trò cảnh báo đa phần cứng. (Hook có thể mở rộng) */
}
