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
#include <acceled.h>
#include <map>

#include <RFM69.h>
#include <SPI.h>

// Addresses for this node. CHANGE THESE FOR EACH NODE!

#define NETWORKID     0   // Must be the same for all nodes
#define MYNODEID      1   // My node ID
#define TONODEID      255   // Destination node ID

// RFM69 frequency, uncomment the frequency of your module:

#define FREQUENCY     RF69_915MHZ

// Use ACKnowledge when sending messages (or not):

#define USEACK        false // Request ACKs or not

// Create a library object for our RFM69HCW module:

RFM69 radio;

AcceLED bloon;

unsigned int id;

void setup()
{
  randomSeed(analogRead(0));

  id = random(99999);
  
  Serial.begin(9600);
  Serial.print("Node ");
  Serial.print(MYNODEID,DEC);
  Serial.println(" ready");
  
  Serial.println(id);

  bloon = AcceLED(12, 5, 6, true);
  
  bloon.begin();

  // Initialize the RFM69HCW:

  bool inited = radio.initialize(FREQUENCY, MYNODEID, NETWORKID);
  if (inited) {
    Serial.println("Inited");
    Serial.println(radio.getFrequency());
    Serial.println(radio.readRSSI());
  } else {
    Serial.println("Not inited...");
  }
  
  radio.setHighPower();
}

unsigned counter = 0;
unsigned long count_millis = 0;
unsigned long state_transition_millis = 0;

int hold_color = 0;

int state = 0;
// state
// 0 - cycling
// 1 - holding

char recbuffer[100];
char sendbuffer[6];

boolean must_send = false;

void loop()
{
  bloon.updateOthers();
  memcpy(sendbuffer, &id, 4);
  
  uint8_t this_color = (counter) % 255;
  if (state == 0)
  {
//    bloon.setColorWheel(this_color);
  }
  else
  {
//    bloon.setColorWheel(hold_color);
  }

  float this_accel = bloon.accelMagnitude();
  sendbuffer[5] =  0;
  if (this_accel > 17 && state == 0)
  {
    state = 1;
    hold_color = this_color;
    state_transition_millis = millis();
    
    // send to other
    Serial.println("sendtry");
    sendbuffer[4] = (char)this_color;
    sendbuffer[5] = 255;
    Serial.println("send");
    
    must_send = true;
  }
  else
  {
    must_send = false;
  }
 
  if (millis() - state_transition_millis > 2000 && state == 1)
  {
    state = 0;
  }
  
  if (radio.receiveDone()) // Got one!
  {

    int dataLen = radio.DATALEN;
    for (byte i = 0; i < radio.DATALEN; i++) {
      recbuffer[i]=(char)radio.DATA[i];
    }
    
    hold_color = (uint8_t)recbuffer[4];
    unsigned int other_id;
    memcpy(&other_id, recbuffer, 4);
    uint8_t tapped = (uint8_t)recbuffer[5];
    Serial.println(tapped);
    if (tapped > 127)
    {
      state = 1;
      Serial.println("lol");
      Serial.println(hold_color);
      Serial.println(other_id);
      state_transition_millis = millis();
    }

//    bloon.setSeen(other_id);
    
    if (radio.ACKRequested())
    {
      radio.sendACK();
    }
  }

  if (millis() - count_millis > 500 || must_send)
  {
    counter++;
    count_millis = millis();
    radio.send(TONODEID, sendbuffer, 6);
  }
}
