/*
 * led_rgb.h
 *
 *  Created on: Mar 17, 2026
 *      Author: retobots
 */
#include <main.h>

#ifndef LIB_MYLIB_DRIVERS_LED_RGB_H_
#define LIB_MYLIB_DRIVERS_LED_RGB_H_

class LedRGB {
public:
    LedRGB();
    void setColor(uint8_t r, uint8_t g, uint8_t b);
    void turnOff();
};



#endif /* LIB_MYLIB_DRIVERS_LED_RGB_H_ */
