#include <acceled.h>

AcceLED::AcceLED(unsigned num_leds_,
                   unsigned led_control_pin_,
                   unsigned accel_select_pin_)
{
    _accel_select_pin = accel_select_pin_;
    _led_control_pin = led_control_pin_;
    _num_leds = num_leds_;

    _time_bumped = 0;
}

void AcceLED::begin()
{
    // acceleration setup
    pinMode(_accel_select_pin, OUTPUT);
    digitalWrite(_accel_select_pin, LOW);
    mma = Adafruit_MMA8451();
    mma.begin(0x1C);
    mma.setRange(MMA8451_RANGE_2_G);

    // led setup
    pixels = Adafruit_NeoPixel(_num_leds,
                               _led_control_pin,
                               NEO_GRB + NEO_KHZ800);
    pixels.begin();
}

void AcceLED::setSolid(uint8_t r,
                       uint8_t g, 
                       uint8_t b)
{
    for (unsigned i = 0; i < _num_leds; i++)
    {
        pixels.setPixelColor(i, pixels.Color(r,g,b));
    }
    pixels.show();
}

void AcceLED::setGradient(uint8_t r1,
                          uint8_t g1,
                          uint8_t b1,
                          uint8_t r2,
                          uint8_t g2,
                          uint8_t b2)
{
    for (unsigned i = 0; i < _num_leds; i++)
    {
        float proportion = float(i)/float(_num_leds);
        int r_this = proportion*(r2-r1) + r1;
        int g_this = proportion*(g2-g1) + g1;
        int b_this = proportion*(b2-b1) + b1;
        pixels.setPixelColor(i, pixels.Color(r_this,g_this,b_this));
    }
    pixels.show();    
}

void AcceLED::getAccel(sensors_event_t* event)
{
    digitalWrite(_accel_select_pin, HIGH);
    mma.begin(0x1D);
    mma.read();
    mma.getEvent(event);
    digitalWrite(_accel_select_pin, LOW);    
}

unsigned AcceLED::bumpTime(float threshold = 13)
{
    float mag = accelMagnitude();
    if (mag > threshold)
    {
        _time_bumped++;
    }
    else
    {
        if (_time_bumped >= 1) _time_bumped--;
    }
    return _time_bumped;
}

float AcceLED::accelMagnitude()
{
    sensors_event_t event;
    getAccel(&event);
    return sqrt(event.acceleration.x * event.acceleration.x +
                event.acceleration.y * event.acceleration.y +
                event.acceleration.z * event.acceleration.z);
}

float magnitude(float x,
                float y,
                float z)
{
    return sqrt(x * x +
                y * y +
                z * z);
}

float AcceLED::angleFromVertical()
{
    sensors_event_t event;
    getAccel(&event);

    float cross_product_x = static_y*event.acceleration.z -
                            static_z*event.acceleration.y;
    float cross_product_y = static_z*event.acceleration.x -
                            static_x*event.acceleration.z;
    float cross_product_z = static_x*event.acceleration.y -
                            static_y*event.acceleration.x;

    float angle = magnitude(cross_product_x, cross_product_y, cross_product_z)/
                  (magnitude(static_x, static_y, static_z) *
                    magnitude(event.acceleration.x, event.acceleration.y, event.acceleration.z));

    static_x -= 0.0001 * (static_x - event.acceleration.x);
    static_y -= 0.0001 * (static_y - event.acceleration.y);
    static_z -= 0.0001 * (static_z - event.acceleration.z);

    return angle;
}

void AcceLED::zeroCalibration()
{
    sensors_event_t event;
    getAccel(&event);

    static_x = event.acceleration.x;
    static_y = event.acceleration.y;
    static_z = event.acceleration.z;
}