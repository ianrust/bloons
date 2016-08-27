#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#include <StandardCplusplus.h>
#include <Wire.h>
#include <Adafruit_MMA8451.h>
#include <Adafruit_Sensor.h>
#include <I2Cdev.h>
#include <MPU6050.h>
#include <map>

#define ACCEL_SCALE 0.00057803468

class OtherData
{
public:
    OtherData();
    void setSeen();
    void setTap();
    unsigned long timeSinceSeen();
private:
    bool _tapped;
    unsigned long _millis_since_seen;
};

class AcceLED
{
public:
    AcceLED(){};
    AcceLED(unsigned num_leds_,
            unsigned led_control_pin_,
            unsigned accel_select_pin_,
            bool wireless);
    void begin();
    void update();
    void checkTap(float threshold);
    void angleSwitch();
    void processedTap();
    void setSolid(uint32_t color);
    void setColorWheelGradient(uint8_t wheel_1, uint8_t wheel_2);
    void setColorWheel(uint8_t wheel_color);
    void getAccel(sensors_event_t* event);
    unsigned bumpTime(float threshold);
    float accelMagnitude();
    float angleFromVertical();
    void setSeen(int id);
    void updateOthers();
private:
    void zeroCalibration();

    Adafruit_MMA8451 mma;
    MPU6050 a;

    Adafruit_NeoPixel pixels;

    unsigned _num_leds,
             _led_control_pin,
             _accel_select_pin;

    bool _wireless;

    unsigned _time_bumped;
    unsigned _debounce_millis;

    bool _being_tapped;
    unsigned long _tap_time;

    float static_x,
          static_y,
          static_z;

    std::map<int, OtherData > others;
};