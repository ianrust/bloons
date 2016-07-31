#include "Balloon.h"

Balloon::Balloon(unsigned int numLeds, unsigned int portNumber) {
	_led = &AcceLED(numLeds, portNumber, 7);
	_led->begin();
}

void Balloon::update() {
	bool tapped = this->was_tapped();
  if (tapped) {
    Serial.println("Tap triggered");
    this->setColor(random(0, 170),random(0, 170),random(0, 170));
  }
}

bool Balloon::was_tapped() {
	// Check timer
	unsigned long time_ms = millis() - this->toggle_millis;

  // I have no idea why, but calling this twice makes it work.
  this->_led->accelMagnitude();
	if (this->_led->accelMagnitude() > this->mag_thresh && time_ms > this->bounce_thresh) {
		this->toggle_millis = millis();
		return true;
	}
	return false;
}

void Balloon::setColor(uint8_t r, uint8_t g, uint8_t b) {
	this->_led->setSolid(r,g,b);	
}
