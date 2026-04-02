/**
 * @file ProtocolManager.hpp
 * @brief Tầng Quản trị Giao thức Truyền thông mã hoá (Datalink Protocol).
 */
#ifndef PROTOCOL_MANAGER_HPP
#define PROTOCOL_MANAGER_HPP

#include "Common/DataStructs.h"
#include <stdint.h>
#include <stddef.h>

class ProtocolManager {
private:
    /**
     * @brief Chuẩn hoá đa thức phân phối Cyclic Redundancy Check tĩnh (Chuẩn hóa CRC-8).
     */
    static const uint8_t m_crc8Table[256];

public:
    /**
     * @brief Vận hành phép tính CRC-8 tuyến tính đảm bảo tính vẹn toàn cho Frame.
     * @param data Trái tim chuỗi Byte thô cần ký kết xác thực.
     * @param length Phạm vi (Chiều dài) khối lượng byte cần kiểm.
     * @return uint8_t Chữ ký Checksum hợp lệ quy đổi từ mạng lưới đa thức nội suy.
     */
    static uint8_t calculateCrc8(const uint8_t* data, size_t length);

    /**
     * @brief Thiết lập bao bì truyền tải hoàn chỉnh sẵn sàng đẩy sang UART DMA hoặc Interrupt.
     * @param packet Gói tin Telemetry tham chiếu tới vị trí địa chỉ thật để đính kèm Magic Code.
     */
    static void packageTelemetry(TelemetryPacket& packet);
    
    /**
     * @brief Giải nén và Thẩm định bảo mật gói điều khiển chuyển xuống từ Tổng bộ (ESP).
     * @param buffer Khối buffer thô nhận được từ giao diện Serial.
     * @param length Độ dài Bytes hợp chuẩn theo bản đồ mạng (Network Topology).
     * @return true Cấu trúc hợp lệ (Frame accepted).
     * @return false Tín hiệu rác, cần hủy ngay (Frame dropped).
     */
    static bool validateIncomingPacket(const uint8_t* buffer, size_t length);
};

#endif // PROTOCOL_MANAGER_HPP
