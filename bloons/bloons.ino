#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#include <Wire.h>
#include <Adafruit_MMA8451.h>
#include <Adafruit_Sensor.h>
#include <MPU6050.h>
#include <acceled.h>

AcceLED al1 = AcceLED(5, 5);

void setup()
{
  al1.begin();
}

boolean toggle1 = false;
unsigned long toggle_millis1 = 0;

void loop()
{
  float mag1 = al1.accelMagnitude();
  float angle1 = 500*al1.angleFromVertical();
  
  if (mag1 > 15 && millis()-toggle_millis1 > 1000)
  {
    toggle1 = !toggle1;
    toggle_millis1 = millis();
  }
  
  if (toggle1)
  {
    al1.setSolid(0,170,min(170,angle1));
  }
  else
  {
    al1.setSolid(170,0,min(170,angle1));
  }
}
