
#include <RGBmatrixPanel.h>
#include <Fonts/hebEng5x7avia.h>

#include "defines.h"
#include "classes.h"

my_RGBmatrixPanel matrix(PANEL_A, PANEL_B, PANEL_C, PANEL_D, CLK, LAT, OE, false, mw);
VoltageMapper iconChangerLeft(ICON_CHANGE_LEFT_PIN, R_CONST_LEFT, true, 5.0);
VoltageMapper iconSetChangerLeft(ICON_SET_CHANGE_LEFT_PIN, R_CONST_SETLEFT, true, 5.0);
VoltageMapper iconChangerRight(ICON_CHANGE_RIGHT_PIN, R_CONST_RIGHT, true, 5.0);
VoltageMapper iconSetChangerRight(ICON_SET_CHANGE_RIGHT_PIN, R_CONST_SETRIGHT, true, 5.0);

VoltageMapper colorSetR(RED_PIN, R_CONST_COLOR_R, true, 5.0);
VoltageMapper colorSetG(GREEN_PIN, R_CONST_COLOR_G, true, 5.0);
VoltageMapper colorSetB(BLUE_PIN, R_CONST_COLOR_B, true, 5.0);

bool paintFlag = false;
bool refreshFlag = true;
bool paintRect = false;
uint16_t lastColor = 0;
int lastColor_R = 0;
int lastColor_G = 0;
int lastColor_B = 0;

void PaintFree() {
  bool flagW = iconChangerLeft.Map_R_to_int(0, R_MAX_LEFT, 0, mw - 1, true );
  bool flagH = iconChangerRight.Map_R_to_int(0, R_MAX_RIGHT, 0, mh - 1, true );
  //PRINTDEBUGC(iconChangerLeft.stateVal); PRINTDEBUGC(iconChangerRight.stateVal); PRINTDEBUGC(flagW); PRINTLNDEBUG(flagH);
  if (flagW || flagH || !paintFlag) {
    matrix.paintFree(iconChangerLeft.stateVal, iconChangerRight.stateVal, paintFlag);
    paintFlag = true;
  }
}

void ButtonName() {
  paintFlag = false;
  refreshFlag = true;
  matrix.fillScreen(0);
  matrix.setTextSize(3);
  matrix.setTextColor(matrix.Wheel(0));
  matrix.setCursor(62, 7);
  char *str = "דורין";
  matrix.printHebStr(str);
  while (!digitalRead(BUTTON_NAME_PIN));
  matrix.fillScreen(0);
  matrix.setTextColor(lastColor);

}

void TurnOff() {
  paintFlag = false;
  refreshFlag = true;
  paintRect = !paintRect;
  //matrix.fillRect(0, 0, mw, mh, matrix.Color333(0, 0, 0));
  matrix.fillScreen(0);
  while (digitalRead(MAIN_PIN));
}


void HandleIcons(bool flagSetLeft, bool flagSetRight) {

  bool flagLeft = iconChangerLeft.Map_R_to_int(0, R_MAX_LEFT, 0, iconSetArraySizeLeft[iconSetChangerLeft.stateVal] - 1, true );
  bool flagRight = iconChangerRight.Map_R_to_int(0, R_MAX_RIGHT, 0, iconSetArraySizeRight[iconSetChangerRight.stateVal] - 1, true );
  bool updateBoard = false;

  if (flagLeft || flagSetLeft || refreshFlag) {
    updateBoard = true;
    matrix.setTextSize(3);
    if (iconSetChangerLeft.stateVal < NUM_OF_ICON_SETS) {
      matrix.display_rgbBitmap(iconSetChangerLeft.stateVal, iconChangerLeft.stateVal, 1);
      PRINTDEBUG("Change Left ----> "); PRINTDEBUG(iconSetChangerLeft.stateVal); PRINTDEBUG(" , "); PRINTLNDEBUG(iconChangerLeft.stateVal)
    }
    else if (iconSetChangerLeft.stateVal == 4)
    {
      char str[] = "א";
      str[1] += iconChangerLeft.stateVal;
      matrix.fillRect(0, 0, RGB_BMP_SIZE, RGB_BMP_SIZE, matrix.Color333(0, 0, 0));
      matrix.setCursor(24, 7);
      matrix.printHeb(str, 2);
      delay(1);
    }
  }

  if (flagRight || flagSetRight || refreshFlag) {
    updateBoard = true;
    matrix.setTextSize(3);
    if (iconSetChangerRight.stateVal < NUM_OF_ICON_SETS) {
      matrix.display_rgbBitmap(iconSetChangerRight.stateVal, iconChangerRight.stateVal, 0);
      PRINTDEBUG("Change Right ----> "); PRINTDEBUG(iconSetChangerRight.stateVal); PRINTDEBUG(" , "); PRINTLNDEBUG(iconChangerRight.stateVal)
    }
    else if (iconSetChangerRight.stateVal == 4)
    {
      matrix.fillRect(RGB_BMP_SIZE, 0, RGB_BMP_SIZE, RGB_BMP_SIZE, matrix.Color333(0, 0, 0));

      if (iconChangerRight.stateVal < 10) {
        char str[] = "0";
        str[0] += iconChangerRight.stateVal;
        matrix.setCursor(40, 7);
        matrix.print(str);
        delay(1);
      }
      else if (iconChangerRight.stateVal < 19)
      {
        char str[] = "10";
        //PRINTDEBUGC(str[0]); PRINTDEBUGC(str[1]); PRINTLNDEBUG(str[2]);
        str[0] += (iconChangerRight.stateVal - 10);
        //PRINTDEBUGC(str[0]); PRINTDEBUGC(str[1]); PRINTLNDEBUG(str[2]);
        matrix.setTextSize(2);
        matrix.setCursor(37, 10);
        matrix.print(str);
        delay(1);
      }
      else {
        char str[] = "100";
        matrix.setTextSize(2);
        matrix.setCursor(33, 10);
        matrix.print(str);
        delay(1);
      }
    }

  }
  if (updateBoard && paintRect) {
    matrix.drawRect(0, 0, mw, mh, lastColor);
  }

}

void ColorPicker() {
  int r, g, b;
  bool flagChanged_r = false;
  bool flagChanged_g = false;
  bool flagChanged_b = false;

  if (digitalRead(RED_SW_PIN)) {
    if (lastColor_R != -1) flagChanged_r = true;
    r = -1;
  }
  else {
    flagChanged_r = colorSetR.Map_R_to_int(0, R_MAX_COLOR_R, 0, 15, true );
    r = colorSetR.stateVal;
    if (lastColor_R == -1) flagChanged_r = true;
  }

  if (digitalRead(GREEN_SW_PIN)) {
    if (lastColor_G != -1) flagChanged_g = true;
    g = -1;
  }
  else {
    flagChanged_g = colorSetG.Map_R_to_int(0, R_MAX_COLOR_G, 0, 15, true );
    g = colorSetG.stateVal;
    if (lastColor_G == -1) flagChanged_g = true;
  }

  if (digitalRead(BLUE_SW_PIN)) {
    if (lastColor_B != -1) flagChanged_b = true;
    b = -1;
  }
  else {
    flagChanged_b = colorSetB.Map_R_to_int(0, R_MAX_COLOR_B, 0, 15, true );
    b = colorSetB.stateVal;
    if (lastColor_B == -1) flagChanged_b = true;
  }

  if (flagChanged_r || flagChanged_g || flagChanged_b) {
    lastColor = matrix.Color444(max(r,1), max(g,1), max(b,1));
    lastColor_R = r;
    lastColor_G = g;
    lastColor_B = b;
    matrix.setTextColor(lastColor);
    matrix.curr_color = lastColor;
    refreshFlag = true;
  }
  //PRINTDEBUGC(r);PRINTDEBUGC(g);PRINTDEBUGC(b);PRINTDEBUGC(flagChanged_r);PRINTDEBUGC(flagChanged_g);PRINTLNDEBUG(flagChanged_b);
}

void setup() {

  matrix.begin();
  if (DEBUG) {
    Serial.begin(115200);
  }

  pinMode(MAIN_PIN, INPUT_PULLUP);
  pinMode(RED_SW_PIN, INPUT_PULLUP);
  pinMode(GREEN_SW_PIN, INPUT_PULLUP);
  pinMode(BLUE_SW_PIN, INPUT_PULLUP);
  pinMode(BUTTON_NAME_PIN, INPUT_PULLUP);

  iconChangerLeft.SetAvgCount(10);
  iconChangerRight.SetAvgCount(10);
  iconSetChangerLeft.SetAvgCount(10);
  iconSetChangerRight.SetAvgCount(10);
  iconSetChangerLeft.stateVal = 0;
  iconSetChangerRight.stateVal = 0;

  matrix.setFont(&hebEng5x7avia);
  matrix.setTextSize(3);     // size 1 == 8 pixels high
  matrix.setTextWrap(false); // Don't wrap at end of line - will do ourselves
  matrix.curr_color =  matrix.Color444(15, 0, 7);

  delay(10);

}



void loop() {

  // Turn On/Off with main pin;
  if (digitalRead(MAIN_PIN)) {
    TurnOff();
  }

  // Turn On/Off name;
  if (!digitalRead(BUTTON_NAME_PIN)) {
    ButtonName();
  }

  //Change colors to letters/numbers
  ColorPicker();

  bool _flagSetLeft  =  iconSetChangerLeft.Map_R_to_int(0, R_MAX_SETLEFT, 0, NUM_OF_SETS - 1, true );
  bool _flagSetRight = iconSetChangerRight.Map_R_to_int(0, R_MAX_SETRIGHT, 0, NUM_OF_SETS - 1, true );

  if (iconSetChangerLeft.stateVal == 5 || iconSetChangerRight.stateVal == 5) {
    // Paint
    PaintFree();
  }
  else {
    // Change icons
    HandleIcons(_flagSetLeft, _flagSetRight);
    paintFlag = false;
    refreshFlag = false;
  }

  //PRINTDEBUG("LEFT: ");PRINTDEBUGC(iconChangerLeft.dynamic_R);PRINTDEBUGC(iconChangerLeft.v_measured) ;PRINTDEBUGC(iconChangerLeft.stateVal) ;
  //PRINTDEBUG("LEFT_S: ");PRINTDEBUGC(iconSetChangerLeft.dynamic_R);PRINTDEBUGC(iconSetChangerLeft.v_measured) ;PRINTDEBUGC(iconSetChangerLeft.stateVal) ;
  //PRINTDEBUG("RIGHT: ");PRINTDEBUGC(iconChangerRight.dynamic_R);PRINTDEBUGC(iconChangerRight.v_measured) ;PRINTDEBUGC(iconChangerRight.stateVal) ;
  //PRINTDEBUG("RIGHT_S: ");PRINTDEBUGC(iconSetChangerRight.dynamic_R);PRINTDEBUGC(iconSetChangerRight.v_measured) ;PRINTDEBUGC(iconSetChangerRight.stateVal)   ;
  //PRINTDEBUG("RED: ");PRINTDEBUGC(colorSetR.dynamic_R);PRINTDEBUGC(colorSetR.v_measured) ;PRINTDEBUGC(colorSetR.stateVal) ;
  //PRINTDEBUG("GREEN: ");PRINTDEBUGC(colorSetG.dynamic_R);PRINTDEBUGC(colorSetG.v_measured) ;PRINTDEBUGC(colorSetG.stateVal) ;
  //PRINTDEBUG("BLUE: ");PRINTDEBUGC(colorSetB.dynamic_R);PRINTDEBUGC(colorSetB.v_measured) ;PRINTDEBUGC(colorSetB.stateVal) ;
  //PRINTLNDEBUG("");
}
