// CHANGE FILENAME TO defines.h 

#define DEBUG 1
#define PRINTDEBUG(STR) \
  {  \
    if (DEBUG) Serial.print(STR); \
  }
#define PRINTLNDEBUG(STR) \
  {  \
    if (DEBUG) Serial.println(STR); \
  }


#define SERIALRX_PIN 10
#define SERIALTX_PIN 11
#define BUSY_PIN 9  //pinvalues -->  0-busy, 1-available
#define VOL_PIN A0
#define PRESSURE_PIN A3

#define NUM_OF_BUTTONS 10
#define BUTTON_PIN_VEC {32,33,34,35,36,37,38,39,40,41}

#define NUM_OF_SWITCHES 5
#define SWITCHES_PIN_VEC {23,24,25,29,30}

#define HALL_PIN 44
#define SERVO_PIN 12
#define FAN_PIN 45
#define LED_PIN 50

#define MIN_VOLUME 0
#define MAX_VOLUME 18
#define INIT_VOL 2
#define NUMPIXELS      60
#define MIN_PRESSURE 300
#define MAX_PRESSURE 700
#define MAX_IDEAL_TIME 1200000

#define BOTTOMLEDS_S 0
#define BOTTOMLEDS_E 17
#define RIGHTLEDS_S 18
#define RIGHTLEDS_E 29
#define TOPLEDS_S 30
#define TOPLEDS_E 47
#define LEFTLEDS_S 48
#define LEFTLEDS_E 59


// definition of MP3RGB class
class MP3RGB {
  public:
    MP3RGB() {
      red = 0;
      green = 0;
      blue = 0;
      mp3_on = false;
    }
    
    MP3RGB(int r, int g, int b) {
      red = r;
      green = g;
      blue = b;
      mp3_on = false;
    }
    
    MP3RGB operator= (MP3RGB color1) {
      this->red = color1.red;
      this->green = color1.green;
      this->blue = color1.blue;
    }
    
    void set_zeros() {
      red = 0;
      green = 0;
      blue = 0;
    }
    
    void set_random(int maxRand) {
      red = random(maxRand);
      green = random(maxRand);
      blue = random(maxRand);
    }
    
    void set_color(int r , int g , int b) {
      red = r;
      green = g;
      blue = b;
    }    

    void begin_move(int _start, int _finish) {
      start = _start;
      finish = _finish;
      ledNum = _start;
      mp3_on = true;
      fwd = true;
    }

    void updateStrip(Adafruit_NeoPixel* pixels) {
      if (mp3_on) {
        for (int i = start; i <= finish; i++) {
          if (i == ledNum) (*pixels).setPixelColor(i, (*pixels).Color(red, green, blue));
          else  (*pixels).setPixelColor(i, (*pixels).Color(0, 0, 0));
        }
        (*pixels).show(); // This sends the updated pixel color to the hardware.
        delay(1);
        if (fwd) ledNum++;
        else ledNum --;
        if (ledNum == finish) fwd = false;
        if (ledNum < start && fwd == false) mp3_on = false;
      }
    }

    int red;
    int green;
    int blue;
    int ledNum;
    int start;
    int finish;
    bool mp3_on;
    bool fwd;

};
