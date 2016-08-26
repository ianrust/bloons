#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#include <Wire.h>
#include <Adafruit_MMA8451.h>
#include <Adafruit_Sensor.h>
#include <I2Cdev.h>
#include <MPU6050.h>
#include <acceled.h>

#define NUM_BLOONS 5

AcceLED bloons[NUM_BLOONS];

void setup()
{
  bloons[0] = AcceLED(12, 3, 2, false);
  bloons[1] = AcceLED(12, 5, 4, false);
  bloons[2] = AcceLED(12, 6, 7, false);
  bloons[3] = AcceLED(12, 9, 8, false);
  bloons[4] = AcceLED(12, 11, 13, false);
  
  for (int i = 0; i < NUM_BLOONS; i++)
  {
    bloons[i].begin();
  }
}

unsigned counter = 0;
unsigned long count_millis = 0;

unsigned long count_tap = 0;

int state = 0;
// 0 - cycling
// 1 - holding/flashing
// 2 - all hold
// 3 - fading back

unsigned long state_transition_millis = 0;

int hold_color;
int hold_balloon_index;

float state_1_millis = 4000;
float state_2_millis = 10000;
float state_3_millis = 3000;

void loop()
{
  int dominant_color = 0;
  float dominant_accel = 0;
  int dominant_balloon_index = 0;
  
  for (int i = 0; i < NUM_BLOONS; i++)
  {
    int this_color = (i * 10 + counter) % 255;
    if (state == 0)
    {
      bloons[i].setColorWheelGradient(this_color, (this_color+128) % 255);
    }
    else if (state == 1)
    {
      if (i == hold_balloon_index)
      {
        bloons[i].setColorWheelGradient(hold_color, (hold_color+128) % 255);
      }
      else
      {
        float weight = (state_1_millis - float(millis() - state_transition_millis)) / state_1_millis;
        if (random(0,10) < 10*weight)
        {
          bloons[i].setColorWheelGradient(this_color, (this_color+128) % 255);
        }
        else
        {
          bloons[i].setColorWheelGradient(hold_color, (hold_color+128) % 255);
        }
      }
    }
    else if (state == 2)
    {
      bloons[i].setColorWheelGradient(hold_color, (hold_color+128) % 255);
    }
    else if (state == 3)
    {
        float weight = (state_3_millis - float(millis() - state_transition_millis)) / state_3_millis;
        int average_color = int(hold_color*(weight) + this_color*(1-weight)) % 255;
        this_color = average_color;
        bloons[i].setColorWheelGradient(average_color, (average_color+128) % 255);
    }

    float this_accel = bloons[i].accelMagnitude();
    if (this_accel > dominant_accel)
    {
      dominant_accel = this_accel;
      dominant_color = this_color;
      dominant_balloon_index = i;
    }
    
  }
  
  
  //state transitions
  if (dominant_accel > 20 && (state == 0 || state == 3))
  {
    state = 2;
    hold_color = dominant_color;
    hold_balloon_index = dominant_balloon_index;
    state_transition_millis = millis();
  }
  
  if (millis() - state_transition_millis > state_1_millis &&
        state == 1)
  {
    state = 2;
    state_transition_millis = millis();
  }
  
  if (millis() - state_transition_millis > state_2_millis &&
        state == 2)
  {
    state = 3;
    state_transition_millis = millis();
  }
  
  if (millis() - state_transition_millis > state_3_millis &&
        state == 3)
  {
    state = 0;
    state_transition_millis = millis();
  }
  
  if (millis() - count_millis > 1)
  {
    counter++;
    count_millis = millis();
  }
}
