#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#include <Wire.h>
#include <Adafruit_MMA8451.h>
#include <Adafruit_Sensor.h>
#include <acceled.h>

AcceLED al1 = AcceLED(9, 6, 8);
AcceLED al2 = AcceLED(12, 5, 7);

void setup()
{
  al1.begin();
  al2.begin();
  Serial.begin(9600);
}

boolean toggle1 = false;
unsigned long toggle_millis1 = 0;
boolean toggle2 = false;
unsigned long toggle_millis2 = 0;

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
  
  
  float mag2 = al2.accelMagnitude();
  float angle2 = 100*al2.angleFromVertical();
  
  if (mag2 > 15 && millis()-toggle_millis2 > 1000)
  {
    toggle2 = !toggle2;
    toggle_millis2 = millis();
  }
  
  if (toggle2)
  {
    al2.setSolid(0,170,min(170,angle2));
  }
  else
  {
    al2.setSolid(170,0,min(170,angle2));
  }
}
