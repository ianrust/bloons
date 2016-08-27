#include <acceled.h>

uint32_t wheel(byte pos) {
  if (pos < 85) {
   return Adafruit_NeoPixel::Color(pos * 3, 255 - pos * 3, 0);
  } else if (pos < 170) {
   pos -= 85;
   return Adafruit_NeoPixel::Color(255 - pos * 3, 0, pos * 3);
  } else {
   pos -= 170;
   return Adafruit_NeoPixel::Color(0, pos * 3, 255 - pos * 3);
  }
}


OtherData::OtherData() : _tapped(false), _millis_since_seen(0)
{
    _millis_since_seen = millis();
}

void OtherData::setSeen()
{
    _millis_since_seen = millis();
}

unsigned long OtherData::timeSinceSeen()
{
    return millis() - _millis_since_seen;
}

void OtherData::setTap()
{
    _tapped = true;
}

AcceLED::AcceLED(unsigned num_leds_,
                   unsigned led_control_pin_,
                   unsigned accel_select_pin_,
                   bool wireless)
{
    _accel_select_pin = accel_select_pin_;
    _led_control_pin = led_control_pin_;
    _num_leds = num_leds_;

    _wireless = wireless;

    _time_bumped = 0;

    _debounce_millis = 200;
    _tap_time = millis();
}

void AcceLED::begin()
{
    //wireless acceleration setup
    if (_wireless)
    {
        a.initialize();
    }
    else
    {
        // bunch acceleration setup
        pinMode(_accel_select_pin, OUTPUT);
        digitalWrite(_accel_select_pin, LOW);
        mma = Adafruit_MMA8451();
        mma.begin(0x1C);
        mma.setRange(MMA8451_RANGE_2_G);
    }

    // led setup
    pixels = Adafruit_NeoPixel(_num_leds,
                               _led_control_pin,
                               NEO_GRB + NEO_KHZ800);
    pixels.begin();
}

void AcceLED::update()
{
    bumpTime(15);
}

void AcceLED::checkTap(float threshold)
{
    float mag = accelMagnitude();

    if (mag > threshold && millis() - _tap_time > _debounce_millis)
    {
        _being_tapped = true;
        _tap_time = millis();
    }
}

void AcceLED::setSeen(int id)
{

    if (others.find(id) == others.end())
    {
        Serial.print("Adding ");
        Serial.println(id);
        OtherData other;
        others.insert(std::make_pair(id, other));
    }
    else
    {
        // Serial.println("Saw ");
        // Serial.println(id);
        others.find(id)->second.setSeen();
    }
}

void AcceLED::updateOthers()
{
    std::map<int, OtherData >::iterator it = others.begin();
    while (it != others.end())
    {
        Serial.print("have: ");
        Serial.println(it->first);
        Serial.print("millis since seen: ");
        Serial.println(it->second.timeSinceSeen());
        if (it->second.timeSinceSeen() > 1000)
        {
            Serial.println("erasing");
            others.erase(it);
        }
        else
        {
            Serial.println("stepping");
            it++;
        }
    }
}

void AcceLED::processedTap()
{
    _being_tapped = false;
}

void AcceLED::setSolid(uint32_t color)
{
    for (unsigned i = 0; i < _num_leds; i++)
    {
        pixels.setPixelColor(i, color);
    }
    pixels.show();
}

void AcceLED::setColorWheelGradient(uint8_t wheel_1, uint8_t wheel_2)
{
    for (unsigned i = 0; i < _num_leds; i++)
    {
        if (i < _num_leds/2)
        {
            pixels.setPixelColor(i, wheel(wheel_1));
        }
        else
        {
            pixels.setPixelColor(i, wheel(wheel_2));
        }
    }

    pixels.show();
}

void AcceLED::setColorWheel(uint8_t wheel_color)
{
    setSolid(wheel(wheel_color));
}

void AcceLED::getAccel(sensors_event_t* event)
{
    if (_wireless)
    {
        int16_t ax, ay, az;
        int16_t gx, gy, gz;
        a.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

        event->acceleration.x = ax*ACCEL_SCALE;
        event->acceleration.y = ay*ACCEL_SCALE;
        event->acceleration.z = az*ACCEL_SCALE;
    }
    else
    {
        digitalWrite(_accel_select_pin, HIGH);
        delay(1);
        mma.begin(0x1D);
        mma.read();
        mma.getEvent(event);
        digitalWrite(_accel_select_pin, LOW); 
    }
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

void AcceLED::angleSwitch()
{
    if (angleFromVertical() > 0.25)
    {
        setSolid(wheel(10));        
    }
    else
    {
        setSolid(wheel(200));
    }
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