#include "joystick_driver.h"
#include <Arduino.h>

/* =============================
   CENTER VALUE (after calibration)
============================= */
static int center_vx = 2048;
static int center_vy = 2048;
static int center_omega = 2048;


/* =============================
   INIT
============================= */
void joystick_init()
{
    pinMode(BTN_1, INPUT_PULLUP);

    joystick_calibrate();
}


/* =============================
   CALIBRATION
============================= */
void joystick_calibrate()
{
    long sum_vx = 0;
    long sum_vy = 0;
    long sum_omega = 0;

    const int samples = 200;

    for(int i = 0; i < samples; i++)
    {
        sum_vx += analogRead(PIN_vx);
        sum_vy += analogRead(PIN_vy);
        sum_omega += analogRead(PIN_omega);

        delay(5);
    }

    center_vx = sum_vx / samples;
    center_vy = sum_vy / samples;
    center_omega = sum_omega / samples;
}


/* =============================
   READ ADC WITH FILTER
============================= */
int readADC(int pin)
{
    long sum = 0;

    for(int i = 0; i < 5; i++)
    {
        sum += analogRead(pin);
        delayMicroseconds(200);
    }

    return sum / 5;
}


/* =============================
   READ JOYSTICK RAW DATA
============================= */
void joystick_read(ControlPacket *Data)
{
    int raw_vx = readADC(PIN_vx);
    int raw_vy = readADC(PIN_vy);
    int raw_omega = readADC(PIN_omega);

    /* trừ center */
    Data->vx = raw_vx - center_vx;
    Data->vy = raw_vy - center_vy;
    Data->omega = raw_omega - center_omega;

    /* button mode */
    Data->mode = !digitalRead(BTN_1);
}


/* =============================
   DEADZONE
============================= */
int applyDeadzone(int value, int dz)
{
    if(abs(value) < dz)
        return 0;
}
    if(value > 0){
        return value - dz;
    }else{
        return value + dz;
}


/* =============================
   NORMALIZE
============================= */
int normalizeAxis(int value)
{
    return map(value, -2048, 2048, -100, 100);
}


/* =============================
   EXPONENTIAL CURVE
============================= */
float applyExpo(float x, float expo)
{
    return expo * x * x * x + (1 - expo) * x;
}