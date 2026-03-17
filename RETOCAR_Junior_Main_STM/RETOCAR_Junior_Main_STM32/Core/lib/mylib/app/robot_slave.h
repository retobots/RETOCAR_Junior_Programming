/*
 * robot_slave.h
 *
 *  Created on: Mar 17, 2026
 *      Author: retobots
 */
#include "main.h"

#ifndef LIB_MYLIB_APP_ROBOT_SLAVE_H_
#define LIB_MYLIB_APP_ROBOT_SLAVE_H_

class RobotSlave
{
public:
    enum SystemState_t
    {
        STATE_INIT,
        STATE_ACTIVE,
        STATE_FAULT
    };
    RobotSlave();
    // void init(UART_HandleTypeDef *huart, ADC_HandleTypeDef *hadc);
    // void update();

private:
    SystemState_t currentState;
    // Các Module thành phần (Aggregation)
    // EncoderManager encoder;
    // LineSensor lineSensor;
    // FilterService filter;
    // ProtocolManager protocol;

    // void handleActiveState();
    // void checkSafety();
};

#endif /* LIB_MYLIB_APP_ROBOT_SLAVE_H_ */
