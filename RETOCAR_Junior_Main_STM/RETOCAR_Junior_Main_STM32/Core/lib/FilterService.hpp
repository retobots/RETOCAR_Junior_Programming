/**
 * @file FilterService.hpp
 * @brief Thư viện dịch vụ cung cấp bộ lọc tín hiệu nhúng cho Robot lõi.
 */
#ifndef FILTER_SERVICE_HPP
#define FILTER_SERVICE_HPP

#include <stdint.h>

class FilterService {
public:
    /**
     * @brief Áp dụng thuật toán Lọc trung bình động lũy thừa (EMA - Exponential Moving Average).
     * @param currentValue Mức điện áp thô vừa quét lược về từ bộ chuyển đổi.
     * @param previousValue Dữ liệu nền hệ thống được bảo lưu tại phiên liền trước.
     * @param alpha Hệ số bù (0.0 -> 1.0) điều chỉnh độ nhạy (alpha nhỏ giúp lọc phẳng nhưng làm trễ pha báo hiệu).
     * @return float Điểm đo sau khi bị ép cân độ nhiễu.
     */
    static float applyEma(float currentValue, float previousValue, float alpha);

    /**
     * @brief Thuật toán phân luồng ngẫu nhiên bằng cách tính trung vị (Median Filter) kết hợp hạn chế ngưỡng biên (Threshold).
     * @param newValue Số liệu xung mới phát hiện (Có khả năng chứa thành phần nhiễu tần số cao).
     * @param buffer Khuôn mẫu chứa 3 kết quả gần nhất, hàm sẽ tái sắp xếp cấu trúc mảng trực tiếp.
     * @param threshold Giới hạn cách biệt cho phép giữa giá trị mới và phần tử trung tâm tĩnh.
     * @return int32_t Biến đổi nội bộ đạt mức độ khả tín.
     */
    static int32_t applyMedianWithThreshold(int32_t newValue, int32_t* buffer, int32_t threshold);

    /**
     * @brief Phương thức bỏ phiếu theo đa số (Majority Voting) cho cảm biến kỹ thuật số.
     * @note Có tác dụng ngăn chặn bóng râm phản chiếu, hay luồng loá đột xuấu quét qua hồng ngoại do chấn động.
     * @param bitBuffer Bộ đếm liên tiếp 3 trạng thái của một bit hồng ngoại dò đường.
     * @return uint8_t 1 (Mức logic High thật sự) hoặc 0.
     */
    static uint8_t applyMajorityVote(uint8_t* bitBuffer);
};

#endif // FILTER_SERVICE_HPP
