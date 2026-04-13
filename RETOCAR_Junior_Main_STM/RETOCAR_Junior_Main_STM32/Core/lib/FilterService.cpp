/**
 * @file FilterService.cpp
 * @brief Triển khai thuật toán xử lý dữ liệu số nhúng với chi phí CPU thấp nhất.
 */
#include "FilterService.hpp"

float FilterService::applyEma(float currentValue, float previousValue, float alpha) {
    /* Ép khuôn hệ số nhạy vào dải giá trị chuẩn chỉnh trước khi tính toán số học Float */
    if (alpha < 0.0f) alpha = 0.0f;
    if (alpha > 1.0f) alpha = 1.0f;
    return (alpha * currentValue) + ((1.0f - alpha) * previousValue);
}

/**
 * @brief Lấy trị tuyệt đối nhẹ tối ưu hoá biên dịch thay thế thư viện toán học <math.h>.
 */
static inline int32_t absoluteValue(int32_t val) {
    return (val < 0) ? -val : val;
}

int32_t FilterService::applyMedianWithThreshold(int32_t newValue, int32_t* buffer, int32_t threshold) {
    /* Đẩy mảng tuyến tính bằng Window-Shift trái nhường vị trí cho giá trị tinh khôi */
    buffer[0] = buffer[1];
    buffer[1] = buffer[2];
    buffer[2] = newValue;

    /* Kiến tạo mảng Clone và sắp xếp bóng nổi (Bubble Sort) trên 3 phần tử cực ngắn */
    int32_t sorted[3] = {buffer[0], buffer[1], buffer[2]};
    if (sorted[0] > sorted[1]) { int32_t temp = sorted[0]; sorted[0] = sorted[1]; sorted[1] = temp; }
    if (sorted[1] > sorted[2]) { int32_t temp = sorted[1]; sorted[1] = sorted[2]; sorted[2] = temp; }
    if (sorted[0] > sorted[1]) { int32_t temp = sorted[0]; sorted[0] = sorted[1]; sorted[1] = temp; }
    
    int32_t median = sorted[1];

    /* Loại trừ xung lượng rác (Spikes) từ nhiễu điện từ do tải xả của 4 động cơ gây ra */
    if (absoluteValue(newValue - median) > threshold) {
        return median; /* Phản hồi dữ liệu bình ổn nếu phát hiện tăng xốc đột biến */
    }

    return newValue;
}

uint8_t FilterService::applyMajorityVote(uint8_t* bitBuffer) {
    /* Giải quyết dội nhiễu vật lý của quang trở ở phạm vi cực nhỏ */
    uint8_t sum = 0;
    if (bitBuffer[0]) sum++;
    if (bitBuffer[1]) sum++;
    if (bitBuffer[2]) sum++;
    
    /* Thiết lập ranh giới quyết định (Decision Boundary) bằng mức > 66% đồng thuận */
    return (sum >= 2) ? 1 : 0;
}
