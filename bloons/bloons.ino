#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#include <Wire.h>
#include <Adafruit_MMA8451.h>
#include <Adafruit_Sensor.h>
#include <stdint.h>
#include "Balloon.h"

Balloon *b;

void setup() {
  Serial.begin(9600);
  b = &Balloon(12, 5);
  b->setColor(200,10,10);
}

void loop() {
  b->update();  
}
