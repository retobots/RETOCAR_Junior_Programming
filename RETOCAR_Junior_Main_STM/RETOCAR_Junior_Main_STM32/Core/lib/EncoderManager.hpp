/**
 * @file EncoderManager.hpp
 * @brief Điều khối phân loại và Tích lũy xung Encoder từ Timer.
 */
#ifndef ENCODER_MANAGER_HPP
#define ENCODER_MANAGER_HPP

#include <stdint.h>

class EncoderManager {
private:
    uint16_t m_prevCounter;       /**< Chu kỳ bộ đếm vòng (16-bit Hardware Timer) phiên trước */
    int32_t  m_accumulatedValue;  /**< Giá trị đại lượng vị trí khổng lồ liên tục (32-bit Memory) */

    int32_t  m_filterBuffer[3];   /**< Bảng dữ liệu cửa sổ cung cấp cho thuật toán Lọc Median */
    int32_t  m_threshold;         /**< Vách ngăn cách ly rác hoặc dội cơ học (Bounce rate) */

public:
    /**
     * @brief Thiết lập một trình xử lý dữ liệu động cơ.
     * @param threshold Mức độ thay đổi bất hợp lý bị loại trừ.
     */
    EncoderManager(int32_t threshold = 100);

    /**
     * @brief Thực hiện tái cấp phát quy trình xử lý về mặc định không khoảng cách tĩnh.
     */
    void init();

    /**
     * @brief Lấy và xử lý chênh lệch quay theo hướng thông minh của số nhị phân bù hai.
     * @param currentCounter Số đo tĩnh trong thanh ghi MCU đo thời điểm hiện tại.
     */
    void update(uint16_t currentCounter);

    /**
     * @brief Đúc xuất số liệu xung sau khi thuật toán nhận định tính tích cực của dịch chuyển.
     * @return int32_t Dòng biến hoàn trả kết quả 32-bit signed integer.
     */
    int32_t getAccumulatedValue() const;

    /**
     * @brief Phục hồi định vị toạ độ tương đối của Robot qua hàm Clear mảng vị trí.
     */
    void reset();
};

#endif // ENCODER_MANAGER_HPP