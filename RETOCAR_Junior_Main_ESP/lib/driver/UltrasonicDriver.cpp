#include "UltrasonicDriver.h"
#include "PinConfig.h"

// để tính toán thời gian
volatile unsigned long echoStartMicros = 0;
volatile unsigned long echoEndMicros = 0;
volatile bool newDataAvailable = false;
volatile float calculatedDistanceCm = 0.0f;

UltrasonicDriver::UltrasonicDriver() {}

void UltrasonicDriver::begin() {
    pinMode(HC_SR04_TRIG_PIN, OUTPUT);
    digitalWrite(HC_SR04_TRIG_PIN, LOW);
    
    pinMode(HC_SR04_ECHO_PIN, INPUT);
    // Gắn ngắt cho chân Echo để bắt sự kiện lên xuống
    attachInterrupt(digitalPinToInterrupt(HC_SR04_ECHO_PIN), UltrasonicDriver::echoISR, CHANGE);
}

void UltrasonicDriver::trigger() {
    // Gửi xung trigger 10us
    digitalWrite(HC_SR04_TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(HC_SR04_TRIG_PIN, LOW);
}

float UltrasonicDriver::getDistanceCm() {
    if (newDataAvailable) {
        unsigned long pulseWidth = echoEndMicros - echoStartMicros;
        // Tốc độ âm thanh ~343 m/s, hoặc 0.0343 cm/us
        // Khoảng cách = (pulseWidth * 0.0343) / 2
        calculatedDistanceCm = pulseWidth * 0.01715f;
        newDataAvailable = false;
    }
    return calculatedDistanceCm;
}

void IRAM_ATTR UltrasonicDriver::echoISR() {
    unsigned long currentMicros = micros();
    if (digitalRead(HC_SR04_ECHO_PIN) == HIGH) { // Cạnh lên
        echoStartMicros = currentMicros;
    } else { // Cạnh xuống
        echoEndMicros = currentMicros;
        // Kiểm tra thời gian
        if (echoEndMicros > echoStartMicros) {
             newDataAvailable = true;
        }
    }
}
