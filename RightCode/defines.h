#define DEBUG 1
#define PRINTDEBUG(STR) \
  {  \
    if (DEBUG) Serial.print(STR); \
  }
#define PRINTDEBUGC(STR) \
  {  \
    if (DEBUG) {Serial.print(STR);Serial.print(" , ");} \
  }  
#define PRINTLNDEBUG(STR) \
  {  \
    if (DEBUG) Serial.println(STR); \
  }

#define BITMAP_FLAG 1
#define HIGH_PROGMEM __attribute__((section(".fini7")))

// ################# PINOUT ######################
#define CLK 11 // USE THIS ON ARDUINO MEGA
#define OE   9
#define LAT 10
#define PANEL_A   A0
#define PANEL_B   A1
#define PANEL_C   A2
#define PANEL_D   A3

#define MAIN_PIN 4
#define RED_SW_PIN 7 
#define GREEN_SW_PIN 6
#define BLUE_SW_PIN 5
#define BUTTON_NAME_PIN 3

#define ICON_CHANGE_LEFT_PIN A4
#define ICON_SET_CHANGE_LEFT_PIN A5 
#define ICON_CHANGE_RIGHT_PIN A6
#define ICON_SET_CHANGE_RIGHT_PIN A7 

#define RED_PIN A8
#define GREEN_PIN A9
#define BLUE_PIN A10


// ################# OTHER ######################


#define mw 64
#define mh 32
#define showImage()           swapBuffers(true)
//#define Color(x,y,z)     Color444(x/16,y/16,z/16)

#define BIN_BMP_SIZE 32
#define RGB_BMP_SIZE 32
#define RGB_BMP_SIZE_SQ 1024

#define R_MAX_LEFT 10000
#define R_MAX_RIGHT 10000
#define R_MAX_SETLEFT 10000
#define R_MAX_SETRIGHT 10000
#define R_MAX_COLOR_R 10000
#define R_MAX_COLOR_G 10000
#define R_MAX_COLOR_B 10000

#define R_CONST_LEFT 8240
#define R_CONST_RIGHT 10000
#define R_CONST_SETLEFT 8240
#define R_CONST_SETRIGHT 8240
#define R_CONST_COLOR_R 8200
#define R_CONST_COLOR_G 8200
#define R_CONST_COLOR_B 9000


#define DELTA_PCT 0.25  // for hysterezis value when dynamic R values are noisy

#define NUM_OF_ICON_SETS 4
#define NUM_OF_SETS 6


int iconSetArraySizeLeft[] = {10,13,13,13,27,mw-1}; // animals cars emoji food + letters + free
int iconSetArraySizeRight[] = {10,11,14,11,20,mh-1}; // animals2 clothes flowers icons + numbers + free
