#include "SlaveComm.h"

SlaveComm slaveComm;

SlaveComm::SlaveComm() {
    m_state = ParserState::WAIT_HEADER;
    m_payloadIndex = 0;
    m_expectedLength = 0;
}

void SlaveComm::begin(uint32_t baudrate) {
    // Tăng kích thước bộ đệm Hardware Buffer nhằm giảm tỷ lệ packet loss 
    // trong trường hợp Core 0 phải bận xử lý gửi dữ liệu ESP-NOW.
    Serial2.setRxBufferSize(256);
    Serial2.begin(baudrate);
}

bool SlaveComm::readData(RobotState &state) {
    bool isPacketReady = false;

    // Đọc liên tục cho tới khi xử lý sạch bộ đệm UART để đạt độ trễ thấp nhất có thể.
    while (Serial2.available() > 0) {
        uint8_t incomingByte = Serial2.read();
        
        switch (m_state) {
            case ParserState::WAIT_HEADER:
                // Theo kiến trúc, 0xAA là chữ ký chiều Slave (STM32) -> Master (ESP32)
                if (incomingByte == HEADER_RX) {
                    m_state = ParserState::WAIT_LENGTH;
                }
                break;

            case ParserState::WAIT_LENGTH:
                m_expectedLength = incomingByte;
                
                // Kiểm tra giới hạn bộ đệm để loại bỏ tình trạng kẹt trạng thái của State Machine
                if (m_expectedLength == 0 || m_expectedLength > UART_MAX_LENGTH) {
                    Serial.printf("[UART] Cảnh báo lỗi độ dài gói tin sai chuẩn: %d bytes\n", m_expectedLength);  // debug
                    m_state = ParserState::WAIT_HEADER;
                } else {
                    m_payloadIndex = 0;
                    m_state = ParserState::RECEIVE_PAYLOAD;
                }
                break;

            case ParserState::RECEIVE_PAYLOAD:
                m_payloadBuffer[m_payloadIndex++] = incomingByte;
                
                if (m_payloadIndex >= m_expectedLength) {
                    m_state = ParserState::WAIT_CHECKSUM;
                }
                break;

            case ParserState::WAIT_CHECKSUM:
                uint8_t calculatedCrc = calculateCrc8(m_payloadBuffer, m_expectedLength);
                
                // Mọi gói dữ liệu hợp lệ bắt buộc đồng bộ mã CRC8 vòng
                if (incomingByte == calculatedCrc) {
                    if (m_expectedLength == RAW_TELEMETRY_SIZE) { 
                        // Dữ liệu nội mạng nên ưu tiên ép kiểu thô memcpy, 
                        // ESP32 và STM32 đều dùng Little-Endian
                        memcpy(&state.tickFL, &m_payloadBuffer[0], 4);
                        memcpy(&state.tickFR, &m_payloadBuffer[4], 4);
                        memcpy(&state.tickRL, &m_payloadBuffer[8], 4);
                        memcpy(&state.tickRR, &m_payloadBuffer[12], 4);
                        
                        uint8_t rawLine = m_payloadBuffer[16];
                        state.lineL2 = bitRead(rawLine, 4);
                        state.lineL1 = bitRead(rawLine, 3);
                        state.lineC  = bitRead(rawLine, 2);
                        state.lineR1 = bitRead(rawLine, 1);
                        state.lineR2 = bitRead(rawLine, 0);
                        
                        memcpy(&state.battery, &m_payloadBuffer[17], 2);
                        
                        isPacketReady = true;
                    }
                } else {
                    // Cảnh báo ở cấp độ code lỗi bộ đệm do nhiễu UART.
                    // Serial.println("[UART] Lỗi lệch CRC Mismatch! Packet Dropped.");
                }
                
                // Trả về đợi bản tin mới không cần biết fail hay pass. 
                // Cấu trúc FSM Push từ slave có lợi ích tự cứu này !
                m_state = ParserState::WAIT_HEADER;
                break;
        }
    }
    return isPacketReady;
}

// Thuật toán kiểm tra vòng CRC8 tiêu chuẩn đa thức chuẩn 0x07 (x^8 + x^2 + x + 1)
uint8_t SlaveComm::calculateCrc8(uint8_t* data, uint8_t length) {
    uint8_t crc = 0x00;
    for (uint8_t i = 0; i < length; i++) {
        crc ^= data[i];
        for (uint8_t j = 0; j < 8; j++) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ 0x07;
            } else {
                crc <<= 1;
            }
        }
    }
    return crc;
}

void SlaveComm::sendCommand(RobotCommand cmd) {
    // Độ lớn lệnh cơ sở của dự án là tĩnh và gói gọn trong 2 byte (Led+Còi)
    uint8_t payload[2];
    payload[0] = cmd.mode;
    payload[1] = cmd.buzzer;
    uint8_t length = sizeof(payload);

    uint8_t crc = calculateCrc8(payload, length);

    // Mã nhận diện gửi luồng UART xuống là 0xBB
    Serial2.write(HEADER_TX);
    Serial2.write(length);
    Serial2.write(payload, length);
    Serial2.write(crc);
}