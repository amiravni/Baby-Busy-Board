/***************************************************

  AMIR AVNI - BASED ON EXAMPLE CODE:
  DFPlayer - A Mini MP3 Player For Arduino
  <https://www.dfrobot.com/index.php?route=product/product&product_id=1121>
*/



#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
#include <Servo.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#include <avr/sleep.h>
#endif

#include "defines.h"


SoftwareSerial mySoftwareSerial(SERIALRX_PIN, SERIALTX_PIN); // RX, TX
Servo myservo;
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);
DFRobotDFPlayerMini myDFPlayer;

void printDetail(uint8_t type, int value);
unsigned long timer = 0;
int vol = INIT_VOL;
int lastVol = INIT_VOL;
int fileName = 1;
bool tmp_busyFlag = false;
bool servoUp = false;
int buttons_pinsVec [NUM_OF_BUTTONS] = BUTTON_PIN_VEC;
int switches_pinsVec [NUM_OF_SWITCHES] = SWITCHES_PIN_VEC;
int bias = 0;
int lastServoVal = 0;
int fanValLeds = 0;
int RGBLeds[3] = {0, 0, 0};
bool mp3_on = false;
int mp3_RGBLeds[3] = {0, 0, 0};
int mp3_RGBLed_num = 0;
long long lastUpdated = millis();

MP3RGB mp3_rgb;

void setup()
{
  mySoftwareSerial.begin(9600);

  if (DEBUG) {
    Serial.begin(115200);

    Serial.println();
    Serial.println(F("DFRobot DFPlayer Mini Demo"));
    Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));

    if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
      Serial.println(F("Unable to begin:"));
      Serial.println(F("1.Please recheck the connection!"));
      Serial.println(F("2.Please insert the SD card!"));
      while (true);
    }
    Serial.println(F("DFPlayer Mini online."));
  }
  else {
    myDFPlayer.begin(mySoftwareSerial);
  }

  myDFPlayer.volume(vol);  //Set volume value. From 0 to 30
  lastVol = vol;
  pinMode(BUSY_PIN, INPUT);
  pinMode(VOL_PIN, INPUT);
  pinMode(HALL_PIN, INPUT_PULLUP);
  pinMode(FAN_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(SERVO_PIN, OUTPUT);

  for (int iii = 0; iii < NUM_OF_BUTTONS ; iii++) {
    pinMode(buttons_pinsVec[iii], INPUT_PULLUP);
  }
  for (int iii = 0; iii < NUM_OF_SWITCHES ; iii++) {
    pinMode(switches_pinsVec[iii], INPUT_PULLUP);
  }

  digitalWrite(FAN_PIN, HIGH);


  for (int j = 0; j <= 255; j = j + 5) {
    for (int i = 0; i < NUMPIXELS; i = i + 3) {
      pixels.setPixelColor(i, pixels.Color(0, 0, j));
    }
    pixels.show(); // This sends the updated pixel color to the hardware.
    delay(10);
  }
  for (int j = 255; j >= 0; j = j - 5) {
    for (int i = 0; i < NUMPIXELS; i = i + 3) {
      pixels.setPixelColor(i, pixels.Color(0, 0, j));
    }
    pixels.show(); // This sends the updated pixel color to the hardware.
    delay(10);
  }

  myservo.attach(SERVO_PIN);  // attaches the servo on pin 9 to the servo object

  pinMode(LED_BUILTIN, OUTPUT); //We use the led on pin 13 to indecate when Arduino is A sleep
  digitalWrite(LED_BUILTIN, HIGH); //turning LED on

}

void loop()
{


  int pressVal = analogRead(PRESSURE_PIN);
  int servoVal = map(min(max(pressVal, MIN_PRESSURE), MAX_PRESSURE), MIN_PRESSURE, MAX_PRESSURE, 20, 180);
  int pressVal_led = map(min(max(pressVal, MIN_PRESSURE), MAX_PRESSURE), MIN_PRESSURE, MAX_PRESSURE, 0, 12);
  
  //PRINTLNDEBUG(servoVal);

  if (servoVal < 120) servoUp = true;
  else servoUp = false;

  myservo.write(servoVal);
  if (lastServoVal != servoVal) {
    lastServoVal = servoVal;
    delay(100);
    updateLeds(&pixels, LEFTLEDS_E, LEFTLEDS_S + pressVal_led, -1, 0, 0, 50);
    updateLeds(&pixels, LEFTLEDS_S + pressVal_led - 1, LEFTLEDS_S, -1, 0, 0, 0);
    lastUpdated = millis();
  }
  else {
    delay(1);
  }

  if (servoUp) return;

  // MP3Player - Handle buttons and volume
  if (!mp3_rgb.mp3_on) { // (digitalRead(BUSY_PIN)) { // MP3 Player is not playing
    for (int iii = 0; iii < NUM_OF_BUTTONS ; iii++) {
      if (!digitalRead(buttons_pinsVec[iii])) {
        myDFPlayer.play(iii + 1 + bias);
        mp3_rgb.set_random(100);
        mp3_rgb.begin_move(BOTTOMLEDS_S, BOTTOMLEDS_E);
        PRINTDEBUG("Playing... ");
        PRINTLNDEBUG(iii + 1 + bias);
        delay(10);
        lastUpdated = millis();
        break;
      }
    }
  }
  //PRINTLNDEBUG(analogRead(VOL_PIN));
  //delay(100);

  //vol = (MAX_VOLUME + 1) - map(analogRead(VOL_PIN), 0, 840, MIN_VOLUME, MAX_VOLUME);
  if (!servoUp) {
      int volTmp = analogRead(VOL_PIN);
      vol = min(MAX_VOLUME, ((5 * volTmp) / (1024 - volTmp )));
      if (vol != lastVol && abs(vol - lastVol) > 1 ) {
        PRINTDEBUG("Volume... ");
        PRINTLNDEBUG(vol);
        PRINTLNDEBUG(analogRead(VOL_PIN));
        myDFPlayer.volume(vol);
        lastVol = vol;
    
        int vol2led = map(vol, MIN_VOLUME, MAX_VOLUME, 0, RIGHTLEDS_E - RIGHTLEDS_S);
    
        for (int iii = RIGHTLEDS_S; iii <= RIGHTLEDS_E; iii++) {
          if (iii <= (RIGHTLEDS_S + vol2led)) {
            if (bias == 0) pixels.setPixelColor(iii, pixels.Color(0, 50, 0));
            else if (bias == 10) pixels.setPixelColor(iii, pixels.Color(50, 0, 0));
          }
          else  pixels.setPixelColor(iii, pixels.Color(0, 0, 0));
        }
        pixels.show(); // This sends the updated pixel color to the hardware.
        lastUpdated = millis();
    
      }
  }
  // update strip if sound is working
  if (mp3_rgb.mp3_on) {
    mp3_rgb.updateStrip(&pixels);
  }

  // MP3Player - Handle Serial
  if (DEBUG && myDFPlayer.available()) {
    printDetail(myDFPlayer.readType(), myDFPlayer.read()); //Print the detail message from DFPlayer to handle different errors and states.
  }

  // change MP3 from 0-9 to 10-19
  if (digitalRead(switches_pinsVec[0])) {
    if (bias == 0) {
      lastVol = 0;
      bias = 10;
      lastUpdated = millis();
    }
  }
  else {
    if (bias == 10) {
      lastVol = 0;
      bias = 0;
      lastUpdated = millis();
    }
  }


  if (!digitalRead(HALL_PIN)  || digitalRead(switches_pinsVec[1]) )  {
    digitalWrite(FAN_PIN, LOW);
    if (fanValLeds < 100)  {
      fanValLeds++;
      lastUpdated = millis();
    }
  }
  else {
    digitalWrite(FAN_PIN, HIGH);
    if (fanValLeds > 0)  {
      fanValLeds--;
      lastUpdated = millis();
    }
  }
  updateLeds(&pixels, BOTTOMLEDS_S, BOTTOMLEDS_E, 2, fanValLeds, 0, 0);

  for (int iii = 0; iii < 3; iii++) {
    if (digitalRead(switches_pinsVec[iii + 2]) )  {
      if (RGBLeds[iii] < 100)  {
        (RGBLeds[iii])++;
        lastUpdated = millis();
      }
    }
    else {
      if (RGBLeds[iii] > 0)  {
        (RGBLeds[iii])--;
        lastUpdated = millis();
      }
    }
  }
  updateLeds(&pixels, TOPLEDS_S, TOPLEDS_E, 3, RGBLeds[0], RGBLeds[1], RGBLeds[2]);

  // if nothing was touched on the board for long time (maybe we forgot to turn off) --> go to sleep mode (1,200,000 msec = 20 minutes)
  if ((millis() - lastUpdated) > MAX_IDEAL_TIME) Going_To_Sleep();

}

void updateLeds(Adafruit_NeoPixel* pixels, int start, int finish, int gap, int r, int g, int b) {
  if (gap > 0) {
    for (int i = start; i <= finish; i = i + gap) {
      (*pixels).setPixelColor(i, (*pixels).Color(r, g, b));
    }
  }
  else {
    for (int i = start; i >= finish; i = i + gap) {
      (*pixels).setPixelColor(i, (*pixels).Color(r, g, b));
    }

  }
  (*pixels).show(); // This sends the updated pixel color to the hardware.
  delay(1);
}


void Going_To_Sleep() {
  sleep_enable();//Enabling sleep mode
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);//Setting the sleep mode, in our case full sleep
  digitalWrite(LED_BUILTIN, LOW); //turning LED off
  digitalWrite(FAN_PIN, HIGH);
  for (int i = 0; i < NUMPIXELS; i++) {
    if (i%3)  pixels.setPixelColor(i, pixels.Color(0, 0, 0));
    else pixels.setPixelColor(i, pixels.Color(0, 0, 5));
  }
  pixels.show(); // This sends the updated pixel color to the hardware.
  delay(1000); //wait a second to allow the led to be turned off before going to sleep
  sleep_cpu();//activating sleep mode
}


void printDetail(uint8_t type, int value) {
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      switch (value) {
        case Busy:
          Serial.println(F("Card not found"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          break;
        case Advertise:
          Serial.println(F("In Advertise"));
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}
