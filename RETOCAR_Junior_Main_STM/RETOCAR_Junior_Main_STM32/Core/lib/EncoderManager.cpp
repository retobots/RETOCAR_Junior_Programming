/**
 * @file EncoderManager.cpp
 * @brief Triển khai chi tiết thuật toán dịch chuyển và tự động giãn Wrap-around cho Timer đếm.
 */
#include "EncoderManager.hpp"
#include "FilterService.hpp"

EncoderManager::EncoderManager(int32_t threshold) 
    : m_prevCounter(0), m_accumulatedValue(0), m_threshold(threshold) {
    for(int i = 0; i < 3; i++) {
        m_filterBuffer[i] = 0;
    }
}

void EncoderManager::init() {
    reset();
}

void EncoderManager::update(uint16_t currentCounter) {
    /* Giải quyết tràn số  sử dụng kĩ thuật ép kiểu */
    int16_t delta = (int16_t)(currentCounter - m_prevCounter);
    
    m_accumulatedValue += delta;
    m_prevCounter = currentCounter;

    /* Áp dụng mô hình màng lọc tĩnh Median kết hợp biên độ để chối bỏ sự gián đoạn 
       trong quá trình đọc luồng tín hiệu số của Encoder. */
    m_accumulatedValue = FilterService::applyMedianWithThreshold(
        m_accumulatedValue, 
        m_filterBuffer, 
        m_threshold
    );
}

int32_t EncoderManager::getAccumulatedValue() const {
    return m_accumulatedValue;
}

void EncoderManager::reset() {
    m_accumulatedValue = 0;
    for(int i = 0; i < 3; i++) {
        m_filterBuffer[i] = 0;
    }
}
