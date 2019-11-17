#include "bitmaps.h"
#include "bitmaps_8bits.h"


//////////////////////////////////////////////// VoltageDivider  ////////////////////////////////////////////////


// definition of AnalogRead class
class VoltageDivider {
  public:
    VoltageDivider();
  
    VoltageDivider(int _pin,float _const_R) {
      analog_pin = _pin;
      const_R = _const_R;
      dyn_R_first = true;
    }
    
    VoltageDivider(int _pin,float _const_R,bool _dyn_R_first) {
      analog_pin = _pin;
      const_R = _const_R;
      dyn_R_first = _dyn_R_first;
    }

    VoltageDivider(int _pin,float _const_R,bool _dyn_R_first, float _v_in) {
      analog_pin = _pin;
      const_R = _const_R;
      dyn_R_first = _dyn_R_first;
      v_in = _v_in;
    }

    void SetAvgCount(int _avgCount) {
      if (_avgCount > 0) avgCount = _avgCount;
    }

    void GetValue_Raw() {
      value = 0;
      for (int idx = 0; idx < avgCount; idx++) value += (float)analogRead(analog_pin);
      value /= (float)avgCount;
    }

    void GetValue_V() {
      GetValue_Raw();
      v_measured = ((float)value / 1024.0) * v_in ;
      
    }

    void GetValue_DynR() {
      GetValue_V();
      if (dyn_R_first) dynamic_R = const_R * ((v_in / v_measured) - 1);
      else dynamic_R = const_R * (v_measured / (v_in - v_measured));
      /*PRINTDEBUG(dynamic_R);
      PRINTDEBUG(" , ");
      PRINTDEBUG(v_measured);
      PRINTDEBUG(" , ");
      PRINTLNDEBUG(value);*/
    }



  int analog_pin;
  float value;
  float dynamic_R;
  float const_R;
  bool dyn_R_first;
  float v_in = 5.0;
  float v_measured;
  int avgCount = 1; 
};

//////////////////////////////////////////////// VoltageMapper  ////////////////////////////////////////////////



// definition of voltageMapper class
class VoltageMapper : public VoltageDivider {
  public:
    VoltageMapper(int _pin,float _const_R,bool _dyn_R_first, float _v_in):VoltageDivider(_pin, _const_R, _dyn_R_first, _v_in){};

     // _dyn_R_first is true  --> v_in ---/\/\dynR/\/\---pin-----/\/\cnstR/\/\---- GND
     // _dyn_R_first is false --> v_in ---/\/\cnstR/\/\---pin-----/\/\dynR/\/\---- GND

     // the use of *max_val+1* instead of *max_val* is so the linear mapping will be correct.
     // for example, 0-10000 to 0-2 without correction would be:
     // 0-4999 --> 0
     // 5000-9999 --> 1
     // 10000 --> 2
     // After correction:
     // 0-3333 --> 0
     // 3334-6666 --> 1
     // 6667-10000+ --> 2
    bool Map_R_to_int(int min_R, int max_R, int min_val, int max_val, bool updateValues ) {
      if (updateValues) GetValue_DynR();
      stateVal =  map(min(max((int)dynamic_R, min_R), max_R), min_R, max_R, min_val, max_val+1);
      if (stateVal == max_val+1) stateVal--; 

      if (stateVal == lastStateVal) return false;
      
      if (stateVal != lastStateVal && abs(stateVal - lastStateVal) == 1 ) 
      {
        float delta = ((max_R - min_R) / (max_val+1 - min_val)) * DELTA_PCT;
        if (abs(lastDynR - dynamic_R) < delta)
        {
          //PRINTDEBUG(lastDynR);PRINTDEBUG(" , ");PRINTDEBUG(dynamic_R);PRINTDEBUG(" , ");PRINTDEBUG(delta);PRINTDEBUG(" , ");
          //PRINTLNDEBUG("Hysterezis");
          return false;
          //return lastStateVal;
        }
        
      }
      if (updateValues) {
        lastStateVal = stateVal;
        lastDynR = dynamic_R;
      }
      return true;
      //return stateVal;
    }

    int stateVal = 0;
    int lastStateVal = -99;
    float lastDynR = -9999;
};

//////////////////////////////////////////////// my_RGBmatrixPanel  ////////////////////////////////////////////////

class my_RGBmatrixPanel: public RGBmatrixPanel {
  public:
    my_RGBmatrixPanel(int _A, int _B, int _C, int _D, int _CLK, int _LAT, int _OE, bool _sw, int _mw):RGBmatrixPanel(_A, _B, _C, _D, _CLK, _LAT, _OE, _sw, _mw){};


    void paintFree(int x,int y,bool cont) {
      if (!cont) {
        this->fillScreen(0);
        lastWidth = x;
        lastHeight = y;
        this->drawPixel(x, y, curr_color);
      }
      else {
        int dx = (x - lastWidth);
        int dy = (y - lastHeight);
        int steps;
        int dir = 1;
        int x_tmp;
        int y_tmp;
        float a;
        if (abs(dx) > abs(dy)) steps = dx;
        else  steps = dy;
        if (steps < 0) dir = -1;
        //PRINTDEBUGC(x);PRINTDEBUGC(lastWidth);PRINTDEBUGC(y);PRINTDEBUGC(lastHeight);PRINTDEBUGC(steps);PRINTLNDEBUG(dir);
        for (int i = 0; i < abs(steps); i++) {
              if (steps == dx) {
                a = (float)dy / (float)dx;
                x_tmp = lastWidth + dir*(i+1);
                y_tmp = lastHeight + round( dir * (float)(i+1) * a );
              }
              else {
                a = (float)dx / (float)dy;
                y_tmp = lastHeight + dir*(i+1);
                x_tmp = lastWidth + round( dir * (float)(i+1) * a );                
              }
              //PRINTDEBUG(" ----> ");PRINTDEBUGC(a);PRINTDEBUGC(x_tmp);PRINTLNDEBUG(y_tmp);
              this->drawPixel(x_tmp, y_tmp, curr_color);
        }
        lastWidth = x;
        lastHeight = y;
      }
    }

 // Convert a BGR 4/4/4 bitmap to RGB 5/6/5 used by Adafruit_GFX
    void fixdrawRGBBitmap(int16_t x, int16_t y, uint16_t *bitmap, int16_t w, int16_t h) {
        uint16_t RGB_bmp_fixed[w * h];
        for (uint16_t pixel=0; pixel<w*h; pixel++) {
            uint8_t r,g,b;
            uint16_t color = bitmap[pixel];//pgm_read_word_far(bitmap + pixel);
    
            //Serial.print(color, HEX);
            b = (color & 0xF00) >> 8;
            g = (color & 0x0F0) >> 4;
            r = color & 0x00F;

            b = map(b, 0, 15, 0, 31);
            g = map(g, 0, 15, 0, 63);
            r = map(r, 0, 15, 0, 31);
    
            RGB_bmp_fixed[pixel] = (r << 11) + (g << 5) + b;
        }
        this->drawRGBBitmap(x, y, RGB_bmp_fixed, w, h);
    }


  void getFarImage(uint32_t ptr, int array_pos, int len) {
    for( int i=0; i<len; i++)   img[i] = pgm_read_word_far( ptr + 2*array_pos*len + (2 * i));
  }

  void display_rgbBitmap(int iconSet, uint8_t bmp_num, bool leftFlag) {

    int bmx = 0; 
    int bmy = 0;

    if (BITMAP_FLAG) {
      if (leftFlag) {
        switch (iconSet) {
        case 0:
            getFarImage(pgm_get_far_address(animals),bmp_num,RGB_BMP_SIZE_SQ);
            break;
        case 1:
            getFarImage(pgm_get_far_address(cars),bmp_num,RGB_BMP_SIZE_SQ);
            break;  
        case 2:
            getFarImage(pgm_get_far_address(emoji),bmp_num,RGB_BMP_SIZE_SQ);
            break;    
        case 3:
            getFarImage(pgm_get_far_address(food),bmp_num,RGB_BMP_SIZE_SQ);
            break;  
        default:
            getFarImage(pgm_get_far_address(animals),bmp_num,RGB_BMP_SIZE_SQ);
            break;                                                 
        }
      }
      else {
        bmx = 32; 
        switch (iconSet) {
        case 0:
            getFarImage(pgm_get_far_address(animals2),bmp_num,RGB_BMP_SIZE_SQ);
            break;
        case 1:
            getFarImage(pgm_get_far_address(clothes),bmp_num,RGB_BMP_SIZE_SQ);
            break;  
        case 2:
            getFarImage(pgm_get_far_address(flowers),bmp_num,RGB_BMP_SIZE_SQ);
            break;    
        case 3:
            getFarImage(pgm_get_far_address(icons),bmp_num,RGB_BMP_SIZE_SQ);
            break;  
        default:
            getFarImage(pgm_get_far_address(animals2),bmp_num,RGB_BMP_SIZE_SQ);
            break;                                                 
        }
      }
    
      fixdrawRGBBitmap(bmx, bmy, img, RGB_BMP_SIZE, RGB_BMP_SIZE);
      this->showImage();
    }
}
/*
void display_bitmap(uint8_t bmp_num, uint16_t color,bool erase) {
    static uint16_t bmx,bmy;
    //Serial.println(bmx);
    //Serial.println(bmy);
    // Clear the space under the bitmap that will be drawn as
    // drawing a single color pixmap does not write over pixels
    // that are nul, and leaves the data that was underneath
    if (erase) this->fillRect(bmx,bmy, bmx+BIN_BMP_SIZE,bmy+BIN_BMP_SIZE, this->Color333(0, 0, 0));
    this->drawBitmap(bmx, bmy, my_mono_bmp[bmp_num], BIN_BMP_SIZE, BIN_BMP_SIZE, color);
    bmx += BIN_BMP_SIZE;
    if (bmx >= mw) bmx = 0;
    if (!bmx) bmy += BIN_BMP_SIZE;
    if (bmy >= mh) bmy = 0;
    this->showImage();
}
*/
void printHeb(char str[], int length)
{
  char ot[(int)(length*1.5)];
  for(int i = 0; i<length-1; i+=2)
  {
    //ot = (str[i])*256+str[i+1];
    ot[i] = str[i];
    ot[i+1] = str[i+1];
    ot[i+2] = 0;
  }
   this->printHebrew(ot);    
 
}

void printHebStr(char str[]) {
  uint8_t w = 0;
  uint8_t b = 0;
  
  for (w=0; str[w*2+b]!=0; w++) {
    this->setTextColor(Wheel(w*2));
    //Serial.println((int)str[w*2+b]);
    if ((int)str[w*2+b] == 32)  {
      this->printHebrew(" ");
      b++;
      w--;
    }
    else printHeb(str+(w*2+b),2);
  }
}

uint16_t Wheel(byte WheelPos) {
  if(WheelPos < 8) {
   return this->Color333(7 - WheelPos, WheelPos, 0);
  } else if(WheelPos < 16) {
   WheelPos -= 8;
   return this->Color333(0, 7-WheelPos, WheelPos);
  } else {
   WheelPos -= 16;
   return this->Color333(0, WheelPos, 7 - WheelPos);
  }
} 

int lastWidth = 0;
int lastHeight = 0;
uint16_t curr_color = this->Color444(0, 0, 0);
uint16_t img[RGB_BMP_SIZE_SQ] = { 0 };

};
