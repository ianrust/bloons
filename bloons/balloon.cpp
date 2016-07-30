#include <balloon.h>

Balloon::Balloon() {
	_led = AcceLED(9,6,8);
	_led.begin();
}

Balloon::update() {
	tapped = this.tapped();
	// Determine color to display
	// (function of mode + input data)


	// update color
	this.setColor()
}


bool Balloon::tapped() {
	// Check timer
	unsigned long time = millis() - this.toggle_millis;
	// Check mag
	float mag = this._led.accelMagnitude();

	// If outside timer + outside mag, this is a tap
	if (mag > this.mag_thresh && time > this.bounce_thresh) {
		this.toggle_millis = millis();
		return true
	}
	return false
}

void Balloon::setColor(uint8_t r,
				  uint8_t g,
				  uint8_t b) {
	_led.setColor(r,g,b);	
}