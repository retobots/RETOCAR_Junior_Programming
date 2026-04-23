#ifndef SLAVE_COMM_HPP
#define SLAVE_COMM_HPP

#include <Arduino.h>
#include "../common/DataStructs.h"

// Trạng thái của bộ giải mã gói tin UART từ Slave
enum class ParserState {
    WAIT_HEADER,
    WAIT_LENGTH,
    RECEIVE_PAYLOAD,
    WAIT_CHECKSUM
};

class SlaveComm {
private:
    ParserState m_state;
    uint8_t m_payloadBuffer[64];
    uint8_t m_payloadIndex;
    uint8_t m_expectedLength;

    // Giao thức Hãng: Định dạng Header và Giới hạn Payload để triệt nhiễu
    static const uint8_t HEADER_RX = 0xAA;
    static const uint8_t HEADER_TX = 0xBB;
    static const uint8_t UART_MAX_LENGTH = 64;
    static const uint8_t RAW_TELEMETRY_SIZE = 19; // 16 bytes(Encoder) + 1 byte(Line) + 2 bytes(Battery)

    // Tính mã CRC vòng tránh rách dữ liệu
    uint8_t calculateCrc8(uint8_t* data, uint8_t length);

public:
    SlaveComm();
    
    // Khởi tạo UART2, được khuyên dùng setRxBufferSize để tăng độ ổn định bộ nhớ
    void begin(uint32_t baudrate = 115200);

    // Giải mã gói tin đẩy từ STM32, Non-blocking an toàn
    bool readData(RobotState &state);
    
    // Đóng gói RobotCommand với mã Header 0xBB hướng gửi xuống thiết bị phụ
    void sendCommand(RobotCommand cmd);
};

// Truy cập theo cấu trúc Object duy nhất Singleton
extern SlaveComm slaveComm;

#endif