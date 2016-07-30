#include <acceled.h>

class Balloon {

public:
Balloon(AcceLED led);
void update();
void setColor(uint8_t r,
              uint8_t g,
              uint8_t b);	
private:	
	AcceLED _led;
	bool tapped();
	unsigned long toggle_millis;
	unsigned long bounce_thresh = 1000;
	unsigned long mag_thresh = 15;
}