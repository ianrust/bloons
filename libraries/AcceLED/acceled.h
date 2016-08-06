#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#include <Adafruit_MMA8451.h>
#include <Adafruit_Sensor.h>
#include <I2Cdev.h>
#include <MPU6050.h>
#include <Wire.h>

#define ACCEL_SCALE 0.00057803468

class AcceLED
{
public:
    AcceLED(unsigned num_leds_,
            unsigned led_control_pin_);
    void begin();
    void setSolid(uint8_t r,
                  uint8_t g,
                  uint8_t b);
    void setGradient(uint8_t r1,
                     uint8_t g1,
                     uint8_t b1,
                     uint8_t r2,
                     uint8_t g2,
                     uint8_t b2);
    void getAccel(sensors_event_t* event);
    unsigned bumpTime(float threshold);
    float accelMagnitude();
    float angleFromVertical();
private:
    void zeroCalibration();

    MPU6050 a;

    Adafruit_NeoPixel pixels;

    unsigned _num_leds,
             _led_control_pin;

    unsigned _time_bumped;

    float static_x,
          static_y,
          static_z;
};