#include <acceled.h>

class Balloon {
  public:
    Balloon(unsigned int numLeds, unsigned int portNumber);
    void update(void);
    void setColor(uint8_t r, uint8_t g, uint8_t b);	
    bool was_tapped();


  private:
  	AcceLED *_led;
  	unsigned long toggle_millis;
  	unsigned long bounce_thresh = 250;
  	unsigned long mag_thresh = 15;

};
